#ifndef DUCK_ELF_H
#define DUCK_ELF_H

#include <stdint.h>

#include <inc/abi.hpp>

namespace ELF {
	struct AppConfig {
		uint64_t memory_hard_limit;
		const char *stdin_ptr;
		uint64_t stdin_size;
		uint64_t stdout_max_size;
		uint64_t stderr_max_size;
	};
	
	struct App {
		uint64_t entry;
		uint64_t rsp;
		uint64_t start_addr;
		uint64_t break_addr;
		uint64_t special_region_start_addr;
		uint64_t special_region_break_addr;
		ABI::DuckInfo_t duckinfo_orig;
		ABI::DuckInfo_t *duckinfo_ptr;      // NULL if app is not 64-bit
		ABI::DuckInfo32_t *duckinfo32_ptr;  // NULL if app is not 32-bit
	};
	
	struct RunResult {
		uint64_t count_inst;  // INST_RETIRED_ANY
		uint64_t clk_thread;  // CPU_CLK_UNHALTED_THREAD
		uint64_t clk_ref_tsc;  // CPU_CLK_UNHALTED_REF_TSC
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
		const AppConfig &config, App &app);
	RunResult run(const App &app, uint64_t time_limit_ns = 0);
}

#endif
