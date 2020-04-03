#ifndef DUCK_ELF_H
#define DUCK_ELF_H

namespace ELF {
	struct App64 {
		uint64_t entry;
		uint64_t rsp;
	};
	
	struct RunResult {
		uint64_t time_tsc;
		uint64_t time_ns;
		uint8_t trap_num;
		int32_t return_code;
	};
	
	bool load(const char *buf, uint32_t len, void *dst, App64 &app);
	RunResult run(const App64 &app, uint64_t time_limit_ns = 0);
}

#endif
