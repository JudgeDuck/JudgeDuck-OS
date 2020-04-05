#ifndef DUCK_ELF_H
#define DUCK_ELF_H

namespace ELF {
	struct App64Config {
		uint64_t memory_hard_limit;
		const char *stdin_ptr;
		uint64_t stdin_size;
		uint64_t stdout_max_size;
		uint64_t stderr_max_size;
	};
	
	// [JudgeDuck-ABI, "Running"]
	struct DuckInfo_t {
		uint64_t abi_version;    // = 21 for version 0.02a
		
		const char *stdin_ptr;   // stdin pointer (read-only contents)
		uint64_t stdin_size;     // stdin size
		
		char *stdout_ptr;        // stdout pointer
		uint64_t stdout_size;    // [IN] stdout maximum size
		                         // [OUT] stdout actual size
		
		char *stderr_ptr;        // stderr pointer
		uint64_t stderr_size;    // [IN] stderr maximum size
		                         // [OUT] stderr actual size
	} __attribute__((packed));
	
	struct App64 {
		uint64_t entry;
		uint64_t rsp;
		uint64_t start_addr;
		uint64_t break_addr;
		uint64_t special_region_start_addr;
		uint64_t special_region_break_addr;
		DuckInfo_t duckinfo_orig;
		DuckInfo_t *duckinfo_ptr;
	};
	
	struct RunResult {
		uint64_t time_tsc;
		uint64_t time_ns;
		uint64_t memory_bytes;
		uint64_t memory_kb;
		uint8_t trap_num;
		int32_t return_code;
		
		// These data should NOT be used in next ELF-load
		char *stdout_ptr;
		uint64_t stdout_size;
		char *stderr_ptr;
		uint64_t stderr_size;
	};
	
	bool load(const char *buf, uint32_t len,
		const App64Config &config, App64 &app);
	RunResult run(const App64 &app, uint64_t time_limit_ns = 0);
}

#endif
