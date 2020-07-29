#include <sys/syscall.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/auxv.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// [JudgeDuck-ABI, "Running"]

struct DuckInfo_t {
	uint32_t abi_version;
	
	const char *stdin_ptr;   // stdin pointer (read-only contents)
	uint32_t stdin_size;     // stdin size
	
	char *stdout_ptr;        // stdout pointer
	uint32_t stdout_size;    // [IN] stdout maximum size
	                         // [OUT] stdout actual size
	
	char *stderr_ptr;        // stderr pointer
	uint32_t stderr_size;    // [IN] stderr maximum size
	                         // [OUT] stderr actual size
} __attribute__((packed));

#define AT_DUCK 0x6b637564   // "duck"

// ==========================

template <class T>
inline T min(const T &a, const T &b) {
	return a < b ? a : b;
}

static bool inited;
static DuckInfo_t *duckinfo;
static uint32_t stdout_max_size;
static uint32_t stderr_max_size;
static uint32_t stdin_curr_pos;
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
	stdout_max_size = duckinfo->stdout_size;
	duckinfo->stdout_size = 0;
	
	// Set up stderr
	stderr_max_size = duckinfo->stderr_size;
	duckinfo->stderr_size = 0;
	
	// Set up heap
	const uint32_t temp = (uint32_t) &_end;
	const uint32_t page_size = getauxval(AT_PAGESZ);
	// Round up to page size
	heap_brk = (char *) ((temp + page_size - 1) / page_size * page_size);
	
	// Set up clock
	// TODO: read freq from duckinfo
	tsc_frequency = 3600000000ull;
}

static inline uint64_t rdtsc() {
	uint64_t ret;
	__asm__ volatile ("rdtsc" : "=A" (ret) : : );
	return ret;
}

struct timespec64 {
	uint64_t tv_sec;
	uint64_t tv_nsec;
};

static long duck_clock_get_time_64(clockid_t clk_id, struct timespec64 *tp) {
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
		const uint32_t available_len = duckinfo->stdin_size - stdin_curr_pos;
		const uint32_t actual_len = min(len, available_len);
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
		
		const uint32_t available_len = stdout_max_size - duckinfo->stdout_size;
		const uint32_t actual_len = min(len, available_len);
		memcpy(duckinfo->stdout_ptr + duckinfo->stdout_size, buf, actual_len);
		duckinfo->stdout_size += actual_len;
		
		return actual_len;
	} else if (fd == 2) {
		// write to stderr
		
		const uint32_t available_len = stderr_max_size - duckinfo->stderr_size;
		const uint32_t actual_len = min(len, available_len);
		memcpy(duckinfo->stderr_ptr + duckinfo->stderr_size, buf, actual_len);
		duckinfo->stderr_size += actual_len;
		
		return actual_len;
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
		st->st_mode = S_IFREG | 0644;
		st->st_size = stdin_size;
		st->st_blksize = 4096;
		st->st_blocks = (stdin_size + 4095) / 4096;
		return 0;
	}
	if (fd == 1) {
		const size_t stdout_size = duckinfo->stdout_size;
		st->st_mode = S_IFREG | 0644;
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

static long duck_exit(int code) {
	// NOTE: specified by JudgeDuck ABI
	__asm__ volatile ("int $0x80" : : "a"(SYS_exit), "b"(code));
	return 0;
}

static int duck_set_thread_area(uint32_t u_info) {
	int ret;
	__asm__ volatile ("int $0x80" : "=a"(ret) : "a"(SYS_set_thread_area), "b"(u_info));
	return ret;
}

extern "C"
long __duck32__syscall_handler(long a1, long a2, long a3, long, long, long, long n) {
	if (!inited) init();
	
	switch (n) {
		case SYS_brk:
			return (long) duck_brk((char *) a1);
		case SYS_clock_gettime64:
			return duck_clock_get_time_64((clockid_t) a1, (struct timespec64 *) a2);
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
		case SYS_set_thread_area:
			return duck_set_thread_area((uint32_t) a1);
		case SYS_ioctl:
			return 0;
		case SYS_fstat:
			return duck_fstat((int) a1, (struct stat *) a2);
		default:
			return -1;
	}
}
