#include <sys/syscall.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/auxv.h>
#include <asm/prctl.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

// [JudgeDuck-ABI, "Running"]

struct DuckInfo_t {
	uint64_t abi_version;
	
	const char *stdin_ptr;   // stdin pointer (read-only contents)
	uint64_t stdin_size;     // stdin size
	
	char *stdout_ptr;        // stdout pointer
	uint64_t stdout_limit;   // [IN] stdout maximum size
	uint64_t stdout_size;    // [OUT] stdout actual size
	
	char *stderr_ptr;        // stderr pointer
	uint64_t stderr_limit;   // [IN] stderr maximum size
	uint64_t stderr_size;    // [OUT] stderr actual size
	
	const char *IB_ptr;      // IB (Input Buffer) pointer
	uint64_t IB_limit;       // IB limit
	
	char *OB_ptr;            // OB (Output Buffer) pointer
	uint64_t OB_limit;       // OB limit
	
	uint64_t tsc_frequency;  // TSC (Time Stamp Counter) frequency
} __attribute__((packed));

#define AT_DUCK 0x6b637564   // "duck"

// ==========================

template <class T>
inline T min(const T &a, const T &b) {
	return a < b ? a : b;
}

static bool inited;
static DuckInfo_t *duckinfo;
static uint64_t stdout_max_size;
static uint64_t stderr_max_size;
static uint64_t stdin_curr_pos;
static uint64_t tsc_frequency;

// WARN: undefined behaviour when the heap hits the stack
extern int _end;
static char *heap_brk;

static void init() {
	inited = true;
	
	// Get duck info
	duckinfo = (DuckInfo_t *) getauxval(AT_DUCK);
	
	// Set up stdin
	stdin_curr_pos = 0;
	
	// Set up stdout
	stdout_max_size = duckinfo->stdout_limit;
	duckinfo->stdout_size = 0;
	
	// Set up stderr
	stderr_max_size = duckinfo->stderr_limit;
	duckinfo->stderr_size = 0;
	
	// Set up heap
	const uint64_t temp = (uint64_t) &_end;
	const uint64_t page_size = getauxval(AT_PAGESZ);
	// Round up to page size
	heap_brk = (char *) ((temp + page_size - 1) / page_size * page_size);
	
	// Set up clock
	tsc_frequency = duckinfo->tsc_frequency;
}

static inline uint64_t rdtsc() {
	uint32_t hi, lo;
	__asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi) : : );
	return (uint64_t) hi << 32 | lo;
}

static long duck_clock_get_time(clockid_t clk_id, struct timespec *tp) {
	// TODO: other clocks
	// TODO: return cpu time
	if (clk_id == CLOCK_PROCESS_CPUTIME_ID) {
		uint64_t t = rdtsc();
		uint64_t sec = t / tsc_frequency;
		uint64_t nsec = t % tsc_frequency * 1000000000ull / tsc_frequency;
		tp->tv_sec = sec;
		tp->tv_nsec = nsec;
		return 0;
	} else {
		return -1;
	}
}

static size_t duck_read(int fd, char *buf, size_t len) {
	if (fd == 0) {
		// read from stdin
		const uint64_t available_len = duckinfo->stdin_size - stdin_curr_pos;
		const uint64_t actual_len = min(len, available_len);
		memcpy(buf, duckinfo->stdin_ptr + stdin_curr_pos, actual_len);
		stdin_curr_pos += actual_len;
		
		return actual_len;
	} else {
		return 0;
	}
}

static size_t duck_readv(int fd, const struct iovec *iov, int cnt) {
	size_t ret = 0;
	for (int i = 0; i < cnt; i++) {
		size_t tmp = duck_read(fd, (char *) iov[i].iov_base, iov[i].iov_len);
		if (tmp <= iov[i].iov_len) {
			ret += tmp;
		}
		if (tmp != iov[i].iov_len) {
			break;
		}
	}
	return ret;
}

static size_t duck_write(int fd, const char *buf, size_t len) {
	if (fd == 1) {
		// write to stdout
		
		const uint64_t available_len = stdout_max_size - duckinfo->stdout_size;
		const uint64_t actual_len = min(len, available_len);
		memcpy(duckinfo->stdout_ptr + duckinfo->stdout_size, buf, actual_len);
		duckinfo->stdout_size += actual_len;
		
		// [2021-09-01] sliently truncate output
		return len;
	} else if (fd == 2) {
		// write to stderr
		
		const uint64_t available_len = stderr_max_size - duckinfo->stderr_size;
		const uint64_t actual_len = min(len, available_len);
		memcpy(duckinfo->stderr_ptr + duckinfo->stderr_size, buf, actual_len);
		duckinfo->stderr_size += actual_len;
		
		// [2021-09-01] sliently truncate output
		return len;
	} else {
		return 0;
	}
}

static size_t duck_writev(int fd, const struct iovec *iov, int cnt) {
	size_t ret = 0;
	for (int i = 0; i < cnt; i++) {
		size_t tmp = duck_write(fd, (const char *) iov[i].iov_base, iov[i].iov_len);
		if (tmp <= iov[i].iov_len) {
			ret += tmp;
		}
		if (tmp != iov[i].iov_len) {
			break;
		}
	}
	return ret;
}

// TODO: real fstat
static int duck_fstat(int fd, struct stat *st) {
	memset(st, 0, sizeof(struct stat));
	if (fd == 0) {
		const size_t stdin_size = duckinfo->stdin_size;
		st->st_mode = S_IFREG | 0664;
		st->st_size = stdin_size;
		st->st_blksize = 4096;
		st->st_blocks = (stdin_size + 4095) / 4096;
		return 0;
	}
	if (fd == 1) {
		const size_t stdout_size = duckinfo->stdout_size;
		st->st_mode = S_IFREG | 0664;
		st->st_size = stdout_size;
		st->st_blksize = 4096;
		st->st_blocks = (stdout_size + 4095) / 4096;
		return 0;
	}
	return -1;
}

static char * duck_brk(char *addr) {
	if (!addr) {
		return heap_brk;
	}
	if (addr > heap_brk) {
		heap_brk = addr;
	}
	return heap_brk;
}

static int duck_arch_prctl(int code, unsigned long *addr) {
	if (code == ARCH_SET_FS) {
		// TODO: use syscall
		__asm__ volatile ("wrfsbase %0" : : "r" (addr));
		return 0;
	}
	
	return -1;
}

static long duck_exit(int code) {
	// NOTE: specified by JudgeDuck ABI
	__asm__ volatile ("syscall" : : "a"(SYS_exit), "D"(code));
	return 0;
}

#include <stdlib.h>

static char __mmap_pool[10240 << 10] __attribute__((aligned(4096)));  // 10MiB
static long __mmap_p = (long) __mmap_pool;

static long duck_mmap_temp(uint64_t size) {
	size = (size + 4095u) & -4096ul;
	long p = __mmap_p;
	__mmap_p += size;
	return p;
}

static long duck_tkill_abort(int sig) {
	// NOTE: kernel interprets this syscall as abort()
	__asm__ volatile ("syscall" : : "a"(SYS_tkill), "d"(sig));
	return 0;
}

static void strncpy_safe(char *dest, const char *src, size_t n) {
	if (n > 0) {
		strncpy(dest, src, n - 1);
		dest[n - 1] = '\0';
	}
}

static long duck_uname(struct utsname *buf) {
	strncpy_safe(buf->sysname, "JudgeDuck", sizeof(buf->sysname));
	strncpy_safe(buf->nodename, "duck", sizeof(buf->nodename));
	
	// glibc _dl_discover_osversion complains about this
	strncpy_safe(buf->release, "5.11.0", sizeof(buf->release));
	
	strncpy_safe(buf->version, "duck-20210901", sizeof(buf->version));
	strncpy_safe(buf->machine, "x86_64", sizeof(buf->machine));
	
	return 0;
}

static long duck_readlink(const char *path, char *buf, int bufsiz) {
	// Mock: copy path to buf
	strncpy_safe(buf, "/tmp/main.exe", bufsiz);
	
	return strlen(path);
}

static pid_t duck_getpid() {
	return 1234;
}

static pid_t duck_gettid() {
	return 1234;
}

extern "C"
long __duck64__syscall_handler(long a1, long a2, long a3, long, long, long, long n) {
	if (!inited) init();
	
	switch (n) {
		case SYS_brk:
			return (long) duck_brk((char *) a1);
		case SYS_clock_gettime:
			return duck_clock_get_time((clockid_t) a1, (struct timespec *) a2);
		case SYS_read:
			return duck_read((int) a1, (char *) a2, (int) a3);
		case SYS_readv:
			return duck_readv((int) a1, (const struct iovec *) a2, (int) a3);
		case SYS_write:
			return duck_write((int) a1, (const char *) a2, (size_t) a3);
		case SYS_writev:
			return duck_writev((int) a1, (const struct iovec *) a2, (int) a3);
		case SYS_exit:
			return duck_exit((int) a1);
		case SYS_exit_group:
			return duck_exit((int) a1);
		case SYS_arch_prctl:
			return (long) duck_arch_prctl((int) a1, (unsigned long *) a2);
		case SYS_ioctl:
			return 0;
		case SYS_fstat:
			return duck_fstat((int) a1, (struct stat *) a2);
		case SYS_mmap:
			return duck_mmap_temp((uint64_t) a2);  // TODO check params!!!
		case SYS_rt_sigprocmask:
			return 0;
		case SYS_rt_sigaction:
			return 0;
		case SYS_sigaltstack:
			return 0;
		case SYS_set_tid_address:
			return 12345;
		case SYS_tkill:  // for abort()
			return duck_tkill_abort((int) a3);
		case SYS_mprotect:
			return 0;  // TODO
		case SYS_munmap:
			return 0;  // TODO
		case SYS_poll:
			return 0;  // TODO (for Rust)
		// [2021-09-01] for glibc
		case SYS_uname:
			return duck_uname((struct utsname *) a1);
		case SYS_readlink:
			return duck_readlink((const char *) a1, (char *) a2, (int) a3);
		case SYS_getpid:
			return duck_getpid();
		case SYS_gettid:
			return duck_gettid();
		case SYS_tgkill:  // glibc abort()
			// Note: tgkill(pid_t tgid, pid_t tid, int sig)
			return duck_tkill_abort((int) a3);
		default:
			return -1;
	}
}
