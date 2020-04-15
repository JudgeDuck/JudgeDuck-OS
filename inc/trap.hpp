#ifndef DUCK_TRAP_H
#define DUCK_TRAP_H

#include <inc/x86_64.hpp>

namespace Trap {
	void init();
	void run_user_64(uint64_t entry, uint64_t rsp, uint64_t time_limit_ns,
		uint64_t &tsc, uint8_t &trap_num, int32_t &return_code);
	void run_user_32(uint64_t entry, uint64_t rsp, uint64_t time_limit_ns,
		uint64_t &tsc, uint8_t &trap_num, int32_t &return_code);
	
	static inline void enable() {
		x86_64::sti();
	}
	
	static inline void disable() {
		x86_64::cli();
	}
}

#endif
