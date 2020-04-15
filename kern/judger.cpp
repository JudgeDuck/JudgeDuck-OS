#include <string.h>

#include <inc/judger.hpp>
#include <inc/logger.hpp>
#include <inc/utils.hpp>
#include <inc/elf.hpp>
#include <inc/memory.hpp>

namespace Judger {
	const uint64_t MAX_ELF_SIZE = 50ul << 20;
	const uint64_t MAX_STDIN_SIZE = 200ul << 20;
	const uint64_t MAX_TIME_LIMIT_NS = 500 * 1e9;  // 500s ??
	
	static char *elf_content;
	static char *stdin_content;
	
	static uint64_t elf_size;
	static uint64_t stdin_size;
	
	static uint64_t judge_seq_num;
	
	static char *stdout_content;
	static char *stderr_content;
	
	static uint64_t stdout_size;
	static uint64_t stderr_size;
	
	static JudgeResult judge_result;
	static bool judge_result_cleared = false;
	
	static void clear_judge_result() {
		if (judge_result_cleared) return;
		judge_result_cleared = true;
		
		judge_seq_num = -1ul;
		
		stdout_content = NULL;
		stderr_content = NULL;
		
		stdout_size = 0;
		stderr_size = 0;
		
		memset(&judge_result, 0, sizeof(judge_result));
		judge_result.error = "Not Judged";
	}
	
	void init() {
		LDEBUG_ENTER_RET();
		
		elf_content = Memory::allocate_virtual_memory(MAX_ELF_SIZE);
		if (!elf_content) {
			LFATAL("Allocate ELF buffer failed");
		}
		
		stdin_content = Memory::allocate_virtual_memory(MAX_STDIN_SIZE);
		if (!stdin_content) {
			LFATAL("Allocate stdin buffer failed");
		}
		
		elf_size = 0;
		stdin_size = 0;
		
		clear_judge_result();
	}
	
	bool set_elf_size(uint64_t size) {
		if (size <= MAX_ELF_SIZE) {
			elf_size = size;
			clear_judge_result();
			return true;
		} else {
			return false;
		}
	}
	
	bool put_elf_data(uint64_t off, const char *data, uint64_t len) {
		if (off <= elf_size && len <= elf_size - off) {
			memcpy(elf_content + off, data, len);
			clear_judge_result();
			return true;
		} else {
			return false;
		}
	}
	
	bool set_stdin_size(uint64_t size) {
		if (size <= MAX_STDIN_SIZE) {
			stdin_size = size;
			clear_judge_result();
			return true;
		} else {
			return false;
		}
	}
	
	bool put_stdin_data(uint64_t off, const char *data, uint64_t len) {
		if (off <= stdin_size && len <= stdin_size - off) {
			memcpy(stdin_content + off, data, len);
			clear_judge_result();
			return true;
		} else {
			return false;
		}
	}
	
	static uint64_t judge_id = 0;
	
	JudgeResult judge(const JudgeConfig &conf) {
		if (conf.seq_num == judge_seq_num) {
			return judge_result;
		}
		
		judge_result_cleared = false;
		
		judge_seq_num = conf.seq_num;
		
		// overflow?
		if (conf.memory_hard_limit_kb * 1024 < conf.memory_hard_limit_kb) {
			judge_result.error = "Can't load ELF";
			return judge_result;
		}
		
		ELF::App app;
		auto elf_conf = (ELF::AppConfig) {
			.memory_hard_limit = conf.memory_hard_limit_kb * 1024,
			.stdin_ptr = stdin_content,
			.stdin_size = stdin_size,
			.stdout_max_size = conf.stdout_max_size,
			.stderr_max_size = conf.stderr_max_size,
		};
		
		// TODO: Check time limit with LAPIC
		if (!conf.time_limit_ns || conf.time_limit_ns > MAX_TIME_LIMIT_NS) {
			judge_result.error = "Invalid time limit";
			return judge_result;
		}
		
		bool r = ELF::load(elf_content, elf_size, elf_conf, app);
		if (!r) {
			judge_result.error = "Can't load ELF";
			return judge_result;
		}
		
		auto res = ELF::run(app, conf.time_limit_ns);
		judge_result = (JudgeResult) {
			.error = NULL,
			.time_ns = res.time_ns,
			.time_tsc = res.time_tsc,
			.memory_kb = res.memory_kb,
			.stdout_size = res.stdout_size,
			.stderr_size = res.stderr_size,
			.return_code = res.return_code,
			.is_RE = res.trap_num != 255,  // TRAP_SYSCALL
			.is_TLE = res.trap_num == 32,  // TRAP_IRQ + IRQ_TIMER
		};
		
		if (res.time_ns > conf.time_limit_ns || judge_result.is_TLE) {
			judge_result.is_RE = false;
			judge_result.is_TLE = true;
			judge_result.time_ns = conf.time_limit_ns;
		}
		
		stdout_size = res.stdout_size;
		stderr_size = res.stderr_size;
		
		stdout_content = res.stdout_ptr;
		stderr_content = res.stderr_ptr;
		
		LINFO("#%lu: time %.6lf ms, mem %lu KiB%s%s",
			++judge_id,
			res.time_ns / 1e6, res.memory_kb,
			judge_result.is_RE ? " (RE)" : "",
			judge_result.is_TLE ? " (TLE)" : "");
		
		return judge_result;
	}
	
	bool get_stdout_data(uint64_t off, uint64_t len, char *data, int &data_len) {
		if (off < stdout_size && len <= stdout_size - off) {
			memcpy(data, stdout_content + off, len);
			data_len = len;
			return true;
		} else {
			return false;
		}
	}
	
	bool get_stderr_data(uint64_t off, uint64_t len, char *data, int &data_len) {
		if (off < stderr_size && len <= stderr_size - off) {
			memcpy(data, stderr_content + off, len);
			data_len = len;
			return true;
		} else {
			return false;
		}
	}
}
