#ifndef DUCK_ELF_H
#define DUCK_ELF_H

namespace ELF {
	struct App64 {
		uint64_t entry;
		uint64_t rsp;
		uint64_t start_addr;
		uint64_t break_addr;
	};
	
	struct RunResult {
		uint64_t time_tsc;
		uint64_t time_ns;
		uint64_t memory_bytes;
		uint64_t memory_kb;
		uint8_t trap_num;
		int32_t return_code;
	};
	
	bool load(const char *buf, uint32_t len, void *dst,
		uint64_t memory_hard_limit, App64 &app);
	RunResult run(const App64 &app, uint64_t time_limit_ns = 0);
}

#endif
