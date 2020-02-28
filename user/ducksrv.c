#include <inc/lib.h>
#include <ip-config.h>
#include <ducknet.h>
#include <inc/x86.h>
#include "duck_welcome.h"
#include "duck_measurements.h"

uint64_t tsc_freq;

inline static void microdelay(uint64_t us) {
	uint64_t tsc_target = read_tsc() + us * (tsc_freq / 1000000);
	while ((long long) (read_tsc() - tsc_target) < 0ll);  // prevent overflow
}

uint32_t metrics[3];

// ==== duck e1000 driver ====

struct TransDesc {
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
} __attribute__((packed));

struct RecvDesc {
	uint64_t addr;
	uint16_t length;
	uint16_t chksum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
} __attribute__((packed));

#define TQ_FLUSH_COUNT 32
#define RQ_FLUSH_COUNT 128

#define RQ_DESC_PAGE_COUNT 1

#define TQSIZE ((PGSIZE) / sizeof(struct TransDesc))
#define RQSIZE ((PGSIZE * RQ_DESC_PAGE_COUNT) / sizeof(struct RecvDesc))
volatile struct TransDesc tq[TQSIZE] __attribute__((aligned(PGSIZE)));
volatile struct RecvDesc rq[RQSIZE] __attribute__((aligned(PGSIZE)));
char tq_pages[TQSIZE][PGSIZE] __attribute__((aligned(PGSIZE)));
char rq_pages[RQSIZE][PGSIZE] __attribute__((aligned(PGSIZE)));
void *tq_addrs[TQSIZE];
void *rq_vaddrs[RQSIZE];
int last_tq_page_id;
int last_tq_page_offset;

volatile char e1000[0x10000] __attribute__((aligned(PGSIZE)));

uint32_t e1000_rdt, e1000_rdh;
uint32_t e1000_rdt_real;
uint32_t e1000_tdt, e1000_tdh;
uint32_t e1000_tdt_real;

int tq_frag_alloc(int cnt, void **va, void **pa) {
	cnt += (-cnt) & (64 - 1);  // ???
	if (last_tq_page_offset + cnt <= PGSIZE) {
		*pa = (char *) tq_addrs[last_tq_page_id] + last_tq_page_offset;
		*va = tq_pages[last_tq_page_id] + last_tq_page_offset;
		last_tq_page_offset += cnt;
		return 0;
	} else if (cnt <= PGSIZE) {
		last_tq_page_id = (last_tq_page_id + 1) % TQSIZE;
		*pa = tq_addrs[last_tq_page_id];
		*va = tq_pages[last_tq_page_id];
		last_tq_page_offset = cnt;
		return 0;
	} else {
		return -1;
	}
}

int e1000_init(unsigned maxMTA) {
	cprintf("e1000 status = %x\n", *(volatile uint32_t *) (e1000 + 0x8));
	
	uint32_t tq_pa, rq_pa;
	int r;
	r = sys_page_alloc_with_pa(0, (void *) tq, PTE_U | PTE_P | PTE_W, &tq_pa);
	if (r < 0) {
		return r;
	}
	
	char *trash_ptr = (char *) 0xe0000000;
	// TODO: No hacking with operating system
	while (1) {
		int fail = 0;
		uint32_t last_pa = 0;
		for (int i = RQ_DESC_PAGE_COUNT - 1; i >= 0; i--) {
			r = sys_page_alloc_with_pa(0, (char *) rq + PGSIZE * i, PTE_U | PTE_P | PTE_W, &rq_pa);
			if (r < 0) {
				return r;
			}
			sys_page_map(0, (char *) rq + PGSIZE * i, 0, trash_ptr, PTE_U | PTE_P);
			trash_ptr += PGSIZE;
			if (last_pa != 0 && last_pa - rq_pa != PGSIZE) {
				fail = 1;
				break;
			}
			last_pa = rq_pa;
		}
		if (!fail) break;
	}
	
	for (int i = 0; i < TQSIZE; i++) {
		uint32_t pa;
		r = sys_page_alloc_with_pa(0, (void *) tq_pages[i], PTE_U | PTE_P | PTE_W, &pa);
		if (r < 0) {
			return r;
		}
		tq_addrs[i] = (void *) pa;
	}
	
	for (int i = 0; i < RQSIZE; i++) {
		if (i % 2 == 1) {
			rq[i].addr = rq[i - 1].addr + PGSIZE / 2;
			rq_vaddrs[i] = (char *) rq_vaddrs[i - 1] + PGSIZE / 2;
			continue;
		}
		uint32_t pa;
		r = sys_page_alloc_with_pa(0, (void *) rq_pages[i], PTE_U | PTE_P | PTE_W, &pa);
		if (r < 0) {
			return r;
		}
		rq[i].addr = pa;
		rq_vaddrs[i] = rq_pages[i];
	}
	
	// For allocating fragments
	last_tq_page_id = 0;
	last_tq_page_offset = 0;
	
	// try reset
	*(volatile uint32_t *) (e1000 + 0xd8) = 0xffffffff;
	uint32_t ctrl = *(volatile uint32_t *) (e1000 + 0x0);
	*(volatile uint32_t *) (e1000 + 0x0) = ctrl | 0x04000000;
	microdelay(1);
	*(volatile uint32_t *) (e1000 + 0xd8) = 0xffffffff;
	
	// try phy reset
	ctrl = *(volatile uint32_t *) (e1000 + 0x0);
	*(volatile uint32_t *) (e1000 + 0x0) = ctrl | 0x80000000;
	microdelay(10000);
	*(volatile uint32_t *) (e1000 + 0x0) = ctrl;
	
	// CTRL: force 1Gbps duplex
	*(volatile uint32_t *) (e1000 + 0x0) = (1u << 0) | (0u << 5) | (1u << 6) | (2u << 8) | (1u << 11) | (1u << 12);
	
	cprintf("e1000: waiting for link ...\n");
	while (1) {
		if ((*(volatile uint32_t *) (e1000 + 0x8)) & 0x2) break;  // link up
		microdelay(100000);
	}
	cprintf("e1000: link up\n");
	
	uint32_t tdbal = tq_pa;
	*(volatile uint32_t *) (e1000 + 0x3800) = tdbal;      // TDBAL
	*(volatile uint32_t *) (e1000 + 0x3804) = 0;          // TDBAH
	*(volatile uint32_t *) (e1000 + 0x3808) = sizeof(tq); // TDLEN
	*(volatile uint32_t *) (e1000 + 0x3810) = 0;          // TDH
	*(volatile uint32_t *) (e1000 + 0x3818) = 0;          // TDT
	e1000_tdt = 0;
	e1000_tdt_real = 0;
	e1000_tdh = 0;
	
	// TXDCTL: GRAN=0 | 1<<22(must=1) | WTHRESH=3 | HTHRESH=4 | PTHRESH=3
	*(volatile uint32_t *) (e1000 + 0x3828) = (0u << 24) | (1u << 22) | (3u << 16) | (4u << 8) | 3u;
	
	// TCTL: EN | PSP | CT=0x10 | COLD=0x40 | MULR
	uint32_t tctl = (1 << 1) | (1 << 3) | (0x10 << 4) | (0x40 << 12) | (1u << 28);
	*(volatile uint32_t *) (e1000 + 0x400) = tctl;        // TCTL
	// ??? *(volatile uint32_t *) (e1000 + 0x410) = 0xa | (0x8 << 10) | (0xc << 20);    // TIPG
	*(volatile uint32_t *) (e1000 + 0x410) = 0x00602006;
	
	uint8_t mac[6] = DUCK_MAC;
	uint32_t RAL = 0, RAH = 0;
	for (int i = 0; i < 4; i++) RAL |= (uint32_t) mac[i] << (i * 8);
	for (int i = 0; i < 2; i++) RAH |= (uint32_t) mac[i + 4] << (i * 8);
	
	*(volatile uint32_t *) (e1000 + 0x5400) = RAL; // RAL
	uint32_t rah = RAH | (1u << 31);
	*(volatile uint32_t *) (e1000 + 0x5404) = rah;        // RAH
	
	for (unsigned i = 0x5200; i < maxMTA; i += 4) {
		*(volatile uint32_t *)(e1000 + i) = 0;                         // MTA
	}
	
	*(volatile uint32_t *) (e1000 + 0xd8) = 0xffffffff;            // IMC
	*(volatile uint32_t *) (e1000 + 0xc0);  // read ICR
	
	*(volatile uint32_t *) (e1000 + 0xd0) = 0;            // IMS
	uint32_t rdbal = rq_pa;
	*(volatile uint32_t *) (e1000 + 0x2800) = rdbal;      // RDBAL
	*(volatile uint32_t *) (e1000 + 0x2804) = 0;          // RDBAH
	*(volatile uint32_t *) (e1000 + 0x2808) = sizeof(rq); // RDLEN
	*(volatile uint32_t *) (e1000 + 0x2810) = 0;          // RDH
	*(volatile uint32_t *) (e1000 + 0x2818) = RQSIZE - 1; // RDT
	e1000_rdh = 0;
	e1000_rdt = RQSIZE - 1;
	e1000_rdt_real = e1000_rdt;
	
	// RXDCTL: GRAN=1 | WTHRESH=4 | HTHRESH=4 | PTHRESH=0x20
	/* not working on I219-V
	*(volatile uint32_t *) (e1000 + 0x2828) = (1u << 24) | (4u << 16) | (4u << 8) | 0x20;
	*/
	
	// RCTL: EN | BAM | BSIZE=2048 | BSEX=0 | SECRC=1
	uint32_t rctl = (1 << 1) | (1 << 15) | (0 << 16) | (0 << 25) | (1 << 26);
	*(volatile uint32_t *) (e1000 + 0x100) = rctl;        // RCTL
	
	cprintf("e1000 status = %x\n", *(volatile uint32_t *) (e1000 + 0x8));
	
	return 0;
}

int network_init() {
	const uint32_t args[][3] = {
		{0x8086, 0x15a3, 0x5280},  // e1000e
		{0x8086, 0x10d3, 0x5280},  // e1000e
		{0x8086, 0x100e, 0x5400},  // e1000
		{0x8086, 0x15bc, 0x5400},  // I219-V
		{0x8086, 0x15b8, 0x5400},  // I219-V H310CM-ITX/ac
		{0, 0, 0}
	};
	
	int r = -1;
	for (int i = 0; args[i][0]; i++) {
		r = sys_map_pci_device(args[i][0], args[i][1], (void *) e1000, sizeof(e1000));
		if (r >= 0) {
			return e1000_init(args[i][2]);
		}
	}
	return r;
}

int network_try_transmit(const void *buf, int cnt) {
	if (cnt < 0 || cnt > 1500) {
		return -1;
	}
	
	if ((e1000_tdt + 1) % TQSIZE == e1000_tdh) {
		e1000_tdh = *(volatile uint32_t *) (e1000 + 0x3810);
	}
	if ((e1000_tdt + 1) % TQSIZE == e1000_tdh) {
		return -1;
	}
	
	void *va, *pa;
	if (tq_frag_alloc(cnt, &va, &pa) < 0) {
		return -1;
	}
	memcpy(va, buf, cnt);
	
	struct TransDesc td;
	memset(&td, 0, sizeof(td));
	td.addr = (uint32_t) pa;
	td.length = cnt;
	td.cmd &= ~(1 << 5);
	td.cmd |= (1 << 3) | (1 << 0);
	td.cmd |= (1 << 1);
	tq[e1000_tdt] = td;
	e1000_tdt = (e1000_tdt + 1) % TQSIZE;
	
	if (e1000_tdt % TQ_FLUSH_COUNT == 0) {
		*(volatile uint32_t *) (e1000 + 0x3818) = e1000_tdt;
		e1000_tdt_real = e1000_tdt;
	}
	
	return 0;
}

int network_flush() {
	if (e1000_tdt != e1000_tdt_real) {
		*(volatile uint32_t *) (e1000 + 0x3818) = e1000_tdt;
		e1000_tdt_real = e1000_tdt;
	}
	
	return 0;
}

// returns: length
int network_try_receive(void *bufpage) {
	uint32_t idx = (e1000_rdt + 1) % RQSIZE;
	volatile struct RecvDesc *rd = rq + idx;
	if (!(rd->status & 1)) {
		return -1;
	}
	int len = rd->length;
	if (len < 0) {
		return -1;
	}
	if (len > PGSIZE) {
		len = PGSIZE;
	}
	memcpy(bufpage, rq_vaddrs[idx], len);
	rd->status &= ~1;
	e1000_rdt = idx;
	if (e1000_rdt % RQ_FLUSH_COUNT == 0) {
		*(volatile uint32_t *) (e1000 + 0x2818) = e1000_rdt;  // ???
		e1000_rdt_real = e1000_rdt;
	}
	return len;
}

// ==== ducksrv ====

#define MAX_ELF_SIZE (20 * 1024 * 1024)

volatile unsigned *start_signal;
volatile unsigned *contestant_ready;
volatile unsigned *contestant_done;

volatile unsigned *contestant_stdout_len;
char *contestant_stdout_content;
char *contestant_stdout_limit;

static const char *trapname(int trapno) {
	static const char * const excnames[] = {
		"Divide error",
		"Debug",
		"Non-Maskable Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalid Opcode",
		"Device Not Available",
		"Double Fault",
		"Coprocessor Segment Overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack Fault",
		"General Protection",
		"Page Fault",
		"(unknown trap)",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine-Check",
		"SIMD Floating-Point Exception"
	};

	if (trapno < ARRAY_SIZE(excnames))
		return excnames[trapno];
	if (trapno == T_SYSCALL)
		return "System call";
	if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16)
		return "Hardware Interrupt";
	return "(unknown trap)";
}

static void wait_or_kill(envid_t envid, int ms) {
	const volatile struct Env *e;

	assert(envid != 0);
	e = &envs[ENVX(envid)];
	uint64_t tsc_end = read_tsc() + tsc_freq / 1000 * ms;
	int ok = 0;
	while (read_tsc() < tsc_end) {
		if (!(e->env_id == envid && e->env_status != ENV_FREE)) {
			ok = 1;
			break;
		}
		sys_yield();
		
		// Wait a small piece of time
		uint64_t tsc_wait = read_tsc() + tsc_freq / 1000 * 10;
		while (read_tsc() < tsc_wait);
	}
	if (!ok) {
		sys_env_destroy(envid);
		
		// Wait 1s to make sure it is killed
		uint64_t tsc_wait = read_tsc() + tsc_freq;
		while (read_tsc() < tsc_wait);
	}
}

#define MAXARGS 16

static void run_arbiter(const char *judging_elf, const char *args) {
	static char argbuf[PGSIZE];
	strncpy(argbuf, args, PGSIZE - 1);
	argbuf[PGSIZE - 1] = 0;
	
	char *argv[MAXARGS + 1];
	int argc = 1;
	argv[0] = "judging";
	
	char *s = argbuf;
	while (1) {
		while (*s == ' ') ++s;
		char c = *s;
		if (c == 0) break;
		if (c == '>') break;
		argv[argc++] = s;
		while (*s != ' ' && *s != 0) ++s;
		*s = 0;
		++s;
		if (argc >= MAXARGS) break;
	}
	argv[argc] = 0;
	
	// Clear contestant stdout
	*contestant_stdout_len = 0;
	
	int fd = open("arbiter.out", O_RDWR | O_CREAT | O_TRUNC);
	if (fd < 0) {
		panic("arbiter(): open arbiter.out failed\n");
		exit();
	}
	
	// Setup contestant signals
	*start_signal = 0;
	*contestant_ready = 0;
	*contestant_done = 0;
	
	envid_t env = spawn_contestant_from_memory(judging_elf, (const char **) argv);
	if (env < 0) {
		cprintf("arbiter(): spawn contestant failed\n");
		fprintf(fd, "verdict = Runtime Error\n");
		fprintf(fd, "time_ms = 0\n");
		fprintf(fd, "mem_kb = 0\n");
		fprintf(fd, "\n");
		fprintf(fd, "Runtime Error: spawn failed\n");
		close(fd);
		return;
	}
	
	sys_yield();
	
	static struct JudgeResult res;
	int ok = 0;
	uint64_t tsc_deadline = read_tsc() + tsc_freq * 10;
	
	while (read_tsc() < tsc_deadline) {
		int ret = sys_accept_enter_judge(env, &res);
		if (ret == 0) {
			ok = 1;
			break;
		}
		sys_yield();
	}
	
	if (!ok) {
		contestant_gg:
		sys_env_destroy(env);
		
		// Wait 1s to make sure it is killed
		uint64_t tsc_wait = read_tsc() + tsc_freq;
		while (read_tsc() < tsc_wait);
		
		cprintf("arbiter(): failed since enter_judge() seems not called\n");
		fprintf(fd, "verdict = Judge Failed\n");
		fprintf(fd, "time_ms = 0\n");
		fprintf(fd, "mem_kb = 0\n");
		fprintf(fd, "\n");
		fprintf(fd, "Judge Failed: enter_judge() seems not called\n");
	} else {
		uint64_t tsc_deadline = read_tsc() + tsc_freq * 5;
		
		// Wait contestant to get ready
		while (*contestant_ready == 0 && read_tsc() < tsc_deadline);
		if (!contestant_ready) goto contestant_gg;
		
		// Wait a small piece of time
		for (int i = 0; i < 100000000; i++) __asm__ volatile("");
		
		// Send start signal
		*start_signal = 1;
		uint64_t tsc_start = read_tsc();
		
		// Wait contestant to finish
		// It must finish even if it dropped into kernel
		while (*contestant_done == 0) __asm__ volatile("pause");
		uint64_t tsc_done = read_tsc();
		
		// Kill contestant
		sys_send_ipi(251);
		
		wait_or_kill(env, 5000);
		
		if (*contestant_done) {
			res.time_cycles = tsc_done - tsc_start;
			uint64_t time_secs = res.time_cycles / tsc_freq;
			uint64_t time_frac = res.time_cycles % tsc_freq;
			res.time_ns = time_secs * 1000000000ull + time_frac * 1000000000ull / tsc_freq;
		} else {
			cprintf("gg contestant_done == 0 ??\n");
			assert(0);
		}
		
		const volatile struct Env *my_env;
		my_env = &envs[ENVX(sys_getenvid())];
		if (res.time_ns > my_env->env_judge_prm.ns) {
			if (res.verdict == 0) {
				res.verdict = VERDICT_TLE;
			}
			res.time_ns = my_env->env_judge_prm.ns;
		}
		
		static const char *verdict_str[] = {
			"Run Finished", "Time Limit Exceeded", "Runtime Error", "Illegal Syscall", "System Error", "Memory Limit Exceeded"
		};
		cprintf("ARBITER: verdict = %s\n", verdict_str[(int) res.verdict]);
		fprintf(fd, "verdict = %s\n", verdict_str[(int) res.verdict]);
		if (res.verdict == VERDICT_IS) {
			cprintf("ARBITER: syscall_id = %d\n", res.tf.tf_regs.reg_eax);
			fprintf(fd, "syscall_id = %d\n", res.tf.tf_regs.reg_eax);
		}
		if (res.verdict == VERDICT_RE) {
			cprintf("ARBITER: runtime error %d (%s)\n", res.tf.tf_trapno, trapname(res.tf.tf_trapno));
			fprintf(fd, "runtime error %d (%s)\n", res.tf.tf_trapno, trapname(res.tf.tf_trapno));
		}
		cprintf("ARBITER: time_Mcycles = %d.%06d\n", (int) (res.time_cycles / 1000000), (int) (res.time_cycles % 1000000));
		cprintf("ARBITER: time_ms = %d.%06d\n", (int) (res.time_ns / 1000000), (int) (res.time_ns % 1000000));
		cprintf("ARBITER: mem_kb = %d\n", res.mem_kb);
		fprintf(fd, "time_ms = %d.%06d\n", (int) (res.time_ns / 1000000), (int) (res.time_ns % 1000000));
		fprintf(fd, "mem_kb = %d\n", res.mem_kb);
		
		int fd_in = open("arbiter.in", O_RDWR);
		static char buf[2048];
		int r = read(fd_in, buf, sizeof(buf));
		if (r > 0) write(fd, buf, r);
		close(fd_in);
	}
	
	close(fd);
	
	cprintf("arbiter(): done\n");
}

typedef struct {
	int size;
	int n_pages;
	int start;
	char md5[50];
	int lru_tag;
	bool to_remove;
} FileData;

DucknetIPv4Address pigeon_ip;
ducknet_u16 pigeon_port;
ducknet_u16 duck_port;

int ducksrv_state;

#define STATE_IDLE 0
#define STATE_RECV_ELF 1
#define STATE_RECV_FILE 2
#define STATE_RECV_OBJECT 3
#define STATE_TO_REBOOT -1

#define MAX_STDOUT_SIZE (64 * 1024 * 1024)
#define MAX_STDOUT_QUERY_LEN 1450

char *judge_pages;
unsigned *first_page;  // The 'idx=1' page
unsigned *second_page;
unsigned *input_pos;
unsigned *input_len;
unsigned *answer_pos;
unsigned *answer_len;
char *contestant_stdout_metadata;
char *judging_elf;
void *meta_start;
FileData *filedata;
void *file_start;
int n_files;

uint64_t reboot_tsc;

inline unsigned get_contestant_stdout_len() {
	unsigned len = *contestant_stdout_len;
	if (len > MAX_STDOUT_SIZE) {
		len = MAX_STDOUT_SIZE;
	}
	return len;
}

void ducksrv_init() {
	ducknet_parse_ipv4(DEFAULT, &pigeon_ip);
	pigeon_port = PIGEON_PORT;
	duck_port = 8000;
	
	judge_pages = (char *) 0xc0000000 - JUDGE_PAGES_SIZE;
	
	int ret = sys_map_judge_pages(judge_pages, 0, JUDGE_PAGES_SIZE);
	cprintf("sys_map_judge_pages returns %d, expected %d\n", ret, JUDGE_PAGES_COUNT);
	memset(judge_pages, 0, JUDGE_PAGES_SIZE);
	
	first_page = (unsigned *) ((char *) judge_pages + PGSIZE);
	second_page = first_page + PGSIZE / sizeof(unsigned);
	input_pos = first_page;
	input_len = first_page + 1;
	start_signal = (volatile unsigned *) ((char *) judge_pages + 0x100);
	contestant_ready = (volatile unsigned *) ((char *) judge_pages + 0x104);
	contestant_done = (volatile unsigned *) ((char *) judge_pages + 0x108);
	answer_pos = second_page;
	answer_len = second_page + 1;
	
	contestant_stdout_metadata = judge_pages + PGSIZE * 3;
	contestant_stdout_len = (volatile unsigned *) contestant_stdout_metadata;
	contestant_stdout_content = contestant_stdout_metadata + PGSIZE;
	contestant_stdout_limit = contestant_stdout_content + MAX_STDOUT_SIZE;
	
	judging_elf = contestant_stdout_limit;
	
	meta_start = judging_elf + MAX_ELF_SIZE;
	filedata = (FileData *) meta_start;
	file_start = meta_start + ROUNDUP(JUDGE_PAGES_COUNT * sizeof(FileData), PGSIZE);
	n_files = 0;
	
	ducksrv_state = STATE_IDLE;
	print_duck_welcome();
}

void ducksrv_send(const char *s, int len) {
	ducknet_udp_send(pigeon_ip, pigeon_port, duck_port, s, len);
}

void ducksrv_send_with_port(const char *s, int len, uint16_t port) {
	ducknet_udp_send(pigeon_ip, port, duck_port, s, len);
}

/* Stop-and-wait Protocol */

int last_recv_seq_num = -1;
char reply_buf[1600];  // larger than MTU is ok
int reply_len = 0;

// Returns: 1 on success, 0 on failure, -1 on repeat packet
inline int process_stop_and_wait_packet(char **s_ref, int *len_ref) {
	char *s = *s_ref;
	int len = *len_ref;
	if (s[0] & 128) {
		int seq_num = ((unsigned) (unsigned char) s[0]) - 128;
		if (seq_num == last_recv_seq_num) {
			ducksrv_send(reply_buf, reply_len);
			return -1;
		} else {
			last_recv_seq_num = seq_num;
			*s_ref = s + 1;
			*len_ref = len - 1;
			return 1;
		}
	} else {
		return 0;
	}
}

inline void clear_stop_and_wait_state() {
	last_recv_seq_num = -1;
	reply_len = 0;
}

inline void set_stop_and_wait_reply(const char *s, int len) {
	memcpy(reply_buf, s, len);
	reply_len = len;
}

/* ====================== */

void ducksrv_send_reply(const char *s, int len) {
	set_stop_and_wait_reply(s, len);
	ducksrv_send(s, len);
}

int recv_file_fd;
FileData *recv_fdata;

void ducksrv_udp_packet_handle(char *s, int len, uint16_t packet_sport) {
	if (ducksrv_state == STATE_TO_REBOOT) {
		return;
	}
	
	if (process_stop_and_wait_packet(&s, &len) < 0) {
		return;
	}
	
	s[len] = 0;
	if (strcmp(s, "reboot") == 0) {
		sys_reboot();
		return;
	}
	if (strcmp(s, "clear") == 0) {
		if (ducksrv_state == STATE_RECV_FILE) {
			close(recv_file_fd);
			sync();
		}
		clear_stop_and_wait_state();
		ducksrv_send_reply("cleared", 7);
		ducknet_flush();
		ducksrv_state = STATE_IDLE;
	} else if (ducksrv_state == STATE_RECV_ELF) {
		s[len] = 0;
		if (s[0] == 'g') {
			int off;
			memcpy(&off, s + 4, 4);
			
			memcpy(judging_elf + off, s + 8, len - 8);
			
			s[0] = 'a';
			ducksrv_send(s, 8);
		} else {
			strcpy(s, "sendFile elf ok");
			ducksrv_send_reply(s, strlen(s));
			ducknet_flush();
			ducksrv_state = STATE_IDLE;
		}
	} else if (ducksrv_state == STATE_RECV_FILE) {
		s[len] = 0;
		if (s[0] == 'g') {
			int off;
			memcpy(&off, s + 4, 4);
			
			seek(recv_file_fd, off);
			write(recv_file_fd, s + 8, len - 8);
			
			s[0] = 'a';
			ducksrv_send(s, 8);
		} else {
			close(recv_file_fd);
			sync();
			strcpy(s, "sendFile ok");
			ducksrv_send_reply(s, strlen(s));
			ducknet_flush();
			ducksrv_state = STATE_IDLE;
		}
	} else if (ducksrv_state == STATE_RECV_OBJECT) {
		s[len] = 0;
		if (s[0] == 'g') {
			int off;
			memcpy(&off, s + 4, 4);
			
			int size = recv_fdata->size;
			if (off >= 0 && off <= size && off + len - 8 <= size) {
				memcpy(judge_pages + recv_fdata->start + off, s + 8, len - 8);
			}
			
			s[0] = 'a';
			ducksrv_send(s, 8);
		} else {
			// Write Ahead Logging !!!
			for (int i = 0; i < n_files; i++) {
				filedata[i].lru_tag++;
			}
			++n_files;
			strcpy(s, "sendobj ok");
			ducksrv_send_reply(s, strlen(s));
			ducknet_flush();
			ducksrv_state = STATE_IDLE;
		}
	} else if (strcmp(s, "query-performance-metrics") == 0) {
		snprintf(s, 100, "metrics: %u %u %u\n", metrics[0], metrics[1], metrics[2]);
		ducksrv_send_with_port(s, strlen(s), packet_sport);
		ducknet_flush();
	} else if (s[0] == 'c') {
		cprintf("command:%s:\n", s + 1);
		s[len] = 0;
		
		// arbiter judging ......
		const char *tmp = "arbiter judging ";
		int tmp_len = strlen(tmp);
		char tmp_back = s[1 + tmp_len];
		s[1 + tmp_len] = 0;
		int res = strcmp(s + 1, tmp);
		s[1 + tmp_len] = tmp_back;
		
		if (res == 0) {
			run_arbiter(judging_elf, s + 1 + tmp_len);
			cprintf("run arbiter ok\n");
			strcpy(s, "run arbiter ok");
		} else {
			int fd = open("tmp", O_RDWR | O_CREAT | O_TRUNC);
			write(fd, s + 1, len - 1);
			write(fd, "\n", 1);
			close(fd);
			sync();
			int r = spawnl("sh", "sh", "-w", "tmp", 0);
			if(r < 0) {
				cprintf("failed to spawn %e\n", r);
			}
			wait(r);
			cprintf("wait ok\n");
			strcpy(s, "runCmd ok");
		}
		ducksrv_send_reply(s, strlen(s));
		ducknet_flush();
	} else if (s[0] == 'o') {
		// open file
		int fd = open(s + 1, O_RDONLY);
		int len = 0;
		s[0] = 'f';
		if (fd < 0) {
			cprintf("Can't open file [%s]\n", s + 1);
		} else {
			len = read(fd, s + 1, 1024);
			if (len < 0) {
				len = 0;
			}
			close(fd);
			sync();
		}
		ducksrv_send_reply(s, len + 1);
		ducknet_flush();
	} else if (strcmp(s, "done") == 0) {
		ducksrv_send_reply("done", 4);
		ducknet_flush();
		static int judge_cnt = 0;
		if (++judge_cnt > 500) {
			cprintf("Rebooting in seconds !!!\n");
			ducksrv_state = STATE_TO_REBOOT;
			reboot_tsc = read_tsc() + tsc_freq * 5;
		}
	} else if (strcmp(s, "stdout-query") == 0) {
		// query stdout metadata (length)
		memcpy(s, "slen", 4);
		unsigned stdout_len = get_contestant_stdout_len();
		memcpy(s + 4, &stdout_len, 4);
		ducksrv_send_reply(s, 8);
		ducknet_flush();
	} else if (strncmp(s, "stdout-get", 10) == 0) {
		// get stdout content
		// "stdout-get" <off> <len>
		if (len < 10 + 4 + 4) {
			return;
		}
		
		unsigned stdout_len = get_contestant_stdout_len();
		unsigned query_off, query_len;
		memcpy(&query_off, s + 10, 4);
		memcpy(&query_len, s + 14, 4);
		
		if (query_off > stdout_len) {
			return;
		}
		if (query_len > MAX_STDOUT_QUERY_LEN || query_off + query_len > stdout_len) {
			return;
		}
		
		// "s content " <off> <len> <content>
		memcpy(s, "s content ", 10);
		// off & len already in packet
		memcpy(s + 18, contestant_stdout_content + query_off, query_len);
		
		ducksrv_send_reply(s, 18 + query_len);
		ducknet_flush();
	} else if (strncmp(s, "setobj_I", 8) == 0) {
		static char md5[50];
		strncpy(md5, s + 8, 49);
		
		for (int i = 0; i < n_files; i++) {
			if (strcmp(filedata[i].md5, md5) == 0) {
				*input_pos = filedata[i].start;
				*input_len = filedata[i].size;
				break;
			}
		}
		
		strcpy(s, "setobj_I ok");
		ducksrv_send_reply(s, strlen(s));
		ducknet_flush();
	} else if (strncmp(s, "setobj_A", 8) == 0) {
		static char md5[50];
		strncpy(md5, s + 8, 49);
		
		for (int i = 0; i < n_files; i++) {
			if (strcmp(filedata[i].md5, md5) == 0) {
				*answer_pos = filedata[i].start;
				*answer_len = filedata[i].size;
				break;
			}
		}
		
		strcpy(s, "setobj_A ok");
		ducksrv_send_reply(s, strlen(s));
		ducknet_flush();
	} else if (s[0] == 'f' && strcmp(s + 1, "judging") == 0) {
		cprintf("[j][recv judging elf!!!]\n");
		
		// Clear elf header
		memset(judging_elf, 0, 512);
		
		ducksrv_send_reply("file", 4);
		ducknet_flush();
		
		ducksrv_state = STATE_RECV_ELF;
	} else if (s[0] == 'f') {
		cprintf("[j][recv sendfile!!!]\n");
		// write file
		
		recv_file_fd = open(s + 1, O_RDWR | O_CREAT | O_TRUNC);
		
		ducksrv_send_reply("file", 4);
		ducknet_flush();
		
		ducksrv_state = STATE_RECV_FILE;
	} else if (strncmp(s, "sendobj ", 8) == 0) {
		int size = *(int *) (s + 8);
		char md5[50];
		strncpy(md5, s + 12, 49);
		int ok = 0;
		for (int i = 0; i < n_files; i++) {
			if (strcmp(filedata[i].md5, md5) == 0) {
				ok = 1;
				break;
			}
		}
		if (ok) {
			strcpy(s, "gg sendobj");
			ducksrv_send_reply(s, strlen(s));
			ducknet_flush();
		} else {
			strcpy(s, "sendobj begin");
			ducksrv_send_reply(s, strlen(s));
			ducknet_flush();
			
			int last_pos = file_start - (void *) judge_pages;
			if (n_files > 0) {
				last_pos = filedata[n_files - 1].start + filedata[n_files - 1].n_pages * PGSIZE;
			}
			if (last_pos + size > JUDGE_PAGES_SIZE) {
				// It's full
				int to_remove_size = last_pos + size - JUDGE_PAGES_SIZE;
				while (to_remove_size > 0) {
					int max_tag = 30;
					int max_tag_id = -1;
					int max_size = -1;
					int max_size_id = -1;
					for (int i = 0; i < n_files; i++) {
						FileData *fdata = filedata + i;
						if (fdata->to_remove) continue;
						if (fdata->lru_tag > max_tag) {
							max_tag = fdata->lru_tag;
							max_tag_id = i;
						}
						if (fdata->n_pages > max_size) {
							max_size = fdata->n_pages;
							max_size_id = i;
						}
					}
					int id = max_tag_id != -1 ? max_tag_id : max_size_id;
					filedata[id].to_remove = true;
					to_remove_size -= filedata[id].n_pages * PGSIZE;
				}
				// Defrag
				int j = 0;
				int pos = file_start - (void *) judge_pages;
				for (int i = 0; i < n_files; i++) {
					if (filedata[i].to_remove) continue;
					if (j < i) {
						filedata[j] = filedata[i];
						memmove(judge_pages + pos, judge_pages + filedata[j].start, filedata[j].n_pages * PGSIZE);
					}
					filedata[j].start = pos;
					pos += filedata[j].n_pages * PGSIZE;
					j++;
				}
				n_files = j;
			}
			last_pos = file_start - (void *) judge_pages;
			if (n_files > 0) {
				last_pos = filedata[n_files - 1].start + filedata[n_files - 1].n_pages * PGSIZE;
			}
			recv_fdata = filedata + n_files;
			recv_fdata->size = size;
			strcpy(recv_fdata->md5, md5);
			recv_fdata->n_pages = ROUNDUP(recv_fdata->size, PGSIZE) / PGSIZE;
			recv_fdata->start = last_pos;
			recv_fdata->lru_tag = 0;
			recv_fdata->to_remove = false;
			
			memset(judge_pages + recv_fdata->start, 0, recv_fdata->n_pages * PGSIZE);
			
			ducksrv_state = STATE_RECV_OBJECT;
		}
	}
}

// ========

int my_idle() {
	if (ducksrv_state == STATE_TO_REBOOT) {
		if (read_tsc() >= reboot_tsc) {
			sys_reboot();
		}
	}
	return 0;
}

int my_packet_handle(void *pkt, int len) {
	return 0;
}

int my_ether_packet_handle(DucknetEtherHeader *hdr, int content_len) {
	return 0;
}

int my_arp_packet_handle(DucknetARPHeader *hdr, int len) {
	return 0;
}

int my_ipv4_packet_handle(DucknetIPv4Header *hdr, int content_len) {
	return 0;
}

int my_icmp_packet_handle(DucknetIPv4Address src, DucknetIPv4Address dst, DucknetICMPHeader *hdr, int len) {
	if (ducksrv_state == STATE_TO_REBOOT) {
		return -1;
	}
	return 0;
}

int my_udp_packet_handle(DucknetIPv4Address src, DucknetIPv4Address dst, DucknetUDPHeader *hdr, int len) {
	if (src.addr == pigeon_ip.addr && hdr->dport == duck_port) {
		ducksrv_udp_packet_handle((char *) (hdr + 1), len, hdr->sport);
		return -1;
	}
	return 0;
}

void umain(int argc, char **argv) {
	binaryname = "ducksrv";
	
	cprintf("ducksrv starting ...\n");
	
	if (sys_get_tsc_frequency(&tsc_freq) < 0) {
		cprintf("GG, get tsc freq failed\n");
		return;
	}
	cprintf("tsc freq = %llu\n", tsc_freq);
	
	do_performance_measurements(metrics);
	microdelay(1000 * 1000);
	
	if (network_init() < 0) {
		cprintf("GG, network init failed\n");
		return;
	}
	cprintf("duck network seems ok\n");
	
	DucknetMACAddress my_mac = (DucknetMACAddress) {.a = DUCK_MAC};
	DucknetIPv4Address my_ip;
	if (ducknet_parse_ipv4(IP, &my_ip) < 0) {
		cprintf("GG, invalid IP address\n");
		return;
	}
	cprintf("IP = %s\n", IP);
	
	DucknetConfig conf = {
		.utils = {
			.tsc_freq = tsc_freq
		},
		.phy = {
			.send = network_try_transmit,
			.recv = network_try_receive,
			.flush = network_flush,
			.packet_handle = my_packet_handle
		},
		.ether = {
			.mac = my_mac,
			.packet_handle = my_ether_packet_handle
		},
		.arp = {
			.packet_handle = my_arp_packet_handle
		},
		.ipv4 = {
			.ip = my_ip,
			.packet_handle = my_ipv4_packet_handle
		},
		.icmp = {
			.packet_handle = my_icmp_packet_handle
		},
		.udp = {
			.packet_handle = my_udp_packet_handle
		},
		.idle = my_idle
	};
	
	if (ducknet_init(&conf) < 0) {
		cprintf("GG, libducknet init failed\n");
		return;
	}
	cprintf("libducknet seems ok\n");
	ducksrv_init();
	ducknet_mainloop();
}
