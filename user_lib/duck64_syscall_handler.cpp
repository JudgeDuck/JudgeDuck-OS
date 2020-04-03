#include <sys/syscall.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <asm/prctl.h>
#include <string.h>

// TODO: real write
static size_t duck_write(int fd, const char *, size_t len) {
	if (fd != 1 && fd != 2) {
		return 0;
	}
	
	return len;
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
		const size_t stdin_size = 0;
		st->st_mode = S_IFREG | 0644;
		st->st_size = stdin_size;
		st->st_blksize = 4096;
		st->st_blocks = (stdin_size + 4095) / 4096;
		return 0;
	}
	if (fd == 1) {
		const size_t stdout_size = 4096;
		st->st_mode = S_IFREG | 0644;
		st->st_size = stdout_size;
		st->st_blksize = 4096;
		st->st_blocks = (stdout_size + 4095) / 4096;
		return 0;
	}
	return -1;
}

// TODO: real heap
const int HEAP_SIZE = 1 << 20;
static char heap[HEAP_SIZE];
static char *heap_brk = heap;

static char * duck_brk(char *addr) {
	if (!addr) {
		return heap_brk;
	}
	if (addr > heap_brk && addr <= heap + HEAP_SIZE) {
		heap_brk = addr;
	}
	return heap_brk;
}

static int duck_arch_prctl(int code, unsigned long *addr) {
	if (code == ARCH_SET_FS) {
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

extern "C"
long __duck64__syscall_handler(long a1, long a2, long a3, long, long, long, long n) {
	switch (n) {
		case SYS_exit:
			return duck_exit((int) a1);
		case SYS_exit_group:
			return duck_exit((int) a1);
		case SYS_brk:
			return (long) duck_brk((char *) a1);
		case SYS_arch_prctl:
			return (long) duck_arch_prctl((int) a1, (unsigned long *) a2);
		case SYS_write:
			return duck_write((int) a1, (const char *) a2, (size_t) a3);
		case SYS_writev:
			return duck_writev((int) a1, (const struct iovec *) a2, (int) a3);
		case SYS_ioctl:
			return 0;
		case SYS_fstat:
			return duck_fstat((int) a1, (struct stat *) a2);
		default:
			return -1;
	}
}
