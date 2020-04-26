#ifndef DUCK_ABI_H
#define DUCK_ABI_H

#include <stdint.h>

namespace ABI {
	const uint64_t version = 31;
	const char * const version_str = "0.03a";
	
	// [JudgeDuck-ABI, "Running"]
	const uint64_t AT_DUCK = 0x6b637564;  // "duck"
	
	// [JudgeDuck-ABI, "Termination"]
	const int DUCK_sys_exit = 60;
	const int DUCK_sys_exit_32 = 1;
	
	// [JudgeDuck-ABI, "System Calls"]
	const int DUCK_sys_set_thread_area = 243;
	
	// [JudgeDuck-ABI, "Running"]
	struct DuckInfo_t {
		uint64_t abi_version;
		
		const char *stdin_ptr;   // stdin pointer (read-only contents)
		uint64_t stdin_size;     // stdin size
		
		char *stdout_ptr;        // stdout pointer
		uint64_t stdout_size;    // [IN] stdout maximum size
		                         // [OUT] stdout actual size
		
		char *stderr_ptr;        // stderr pointer
		uint64_t stderr_size;    // [IN] stderr maximum size
		                         // [OUT] stderr actual size
	} __attribute__((packed));
	
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
}

#endif
