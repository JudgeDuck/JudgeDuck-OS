#include <stdint.h>

// [JudgeDuck-ABI, "Running"]
const uint32_t AT_DUCK = 0x6b637564;  // "duck"

// [JudgeDuck-ABI, "Running"]
// Note that 32-bit pointers can not be pointers in 64-bit kernel
struct DuckInfo32_t {
	uint32_t abi_version;
	
	uint32_t stdin_ptr;      // stdin pointer (read-only contents)
	uint32_t stdin_size;     // stdin size
	
	uint32_t stdout_ptr;     // stdout pointer
	uint32_t stdout_size;    // [IN] stdout maximum size
	                         // [OUT] stdout actual size
	
	uint32_t stderr_ptr;     // stderr pointer
	uint32_t stderr_size;    // [IN] stderr maximum size
	                         // [OUT] stderr actual size
} __attribute__((packed));

static int my_strcpy(char *dst, const char *src) {
	int ret = 0;
	while (*src) {
		*dst = *src;
		dst++;
		src++;
		ret++;
	}
	return ret;
}

extern "C"
void _start(int argc) {
	__asm__ volatile ("pushl %esp");  // i386 ABI says argc is at 4(%esp)
	
	DuckInfo32_t *duckinfo = 0;
	
	int *tmp = &argc + 1;
	while (*tmp) tmp++;
	tmp += 1;
	while (*tmp) tmp++;
	tmp += 1;
	for (uint32_t *auxv = (uint32_t *) tmp; auxv[0]; auxv += 2) {
		if (auxv[0] == AT_DUCK) {
			duckinfo = (DuckInfo32_t *) auxv[1];
		}
	}
	
	if (!duckinfo) {
		__asm__ volatile ("int3");
	}
	
	char *duck_stdout = (char *) duckinfo->stdout_ptr;
	int cnt = my_strcpy(duck_stdout, "stdout from 32-bit userspace!");
	duckinfo->stdout_size = cnt;
	
	char *duck_stderr = (char *) duckinfo->stderr_ptr;
	cnt = my_strcpy(duck_stderr, "stderr from 32-bit userspace!");
	duckinfo->stderr_size = cnt;
	
	__asm__ volatile ("int $0x80" : : "a" (1), "b" (321));
}
