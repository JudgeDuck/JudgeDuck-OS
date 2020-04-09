#ifndef DUCK_JUDGER_H
#define DUCK_JUDGER_H

#include <stdint.h>

namespace Judger {
	struct JudgeConfig {
		uint64_t time_limit_ns;
		uint64_t memory_hard_limit_kb;
		uint64_t stdout_max_size;
		uint64_t stderr_max_size;
		uint64_t seq_num;
	};
	
	struct JudgeResult {
		const char *error;
		uint64_t time_ns;
		uint64_t time_tsc;
		uint64_t memory_kb;
		uint64_t stdout_size;
		uint64_t stderr_size;
		int32_t return_code;
		bool is_RE;  // Runtime Error
		bool is_TLE;  // Time Limit Exceeded
	};
	
	void init();
	
	bool set_elf_size(uint64_t size);
	bool put_elf_data(uint64_t off, const char *data, uint64_t len);
	
	bool set_stdin_size(uint64_t size);
	bool put_stdin_data(uint64_t off, const char *data, uint64_t len);
	
	JudgeResult judge(const JudgeConfig &conf);
	
	bool get_stdout_data(uint64_t off, uint64_t len, char *data, int &data_len);
	
	bool get_stderr_data(uint64_t off, uint64_t len, char *data, int &data_len);
}

#endif
