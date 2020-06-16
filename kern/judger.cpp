#include <string.h>

#include <inc/judger.hpp>
#include <inc/logger.hpp>
#include <inc/utils.hpp>
#include <inc/elf.hpp>
#include <inc/memory.hpp>

namespace Judger {	
	// Statistics
	static uint64_t n_judges;
	static uint64_t total_time_ns;
	
	// Buffer
	const uint64_t INITIAL_BUFFER_SIZE = 3072ul << 20;
	// TODO: resizable buffer
	// TODO: smaller initial buffer
	static char *buffer;
	static uint64_t buffer_size;
	
	// Judge
	const uint64_t MAX_TIME_LIMIT_NS = 500 * 1e9;  // 500s ??
	static uint64_t judge_seq_num;
	static uint64_t stdout_size;
	static uint64_t stderr_size;
	static JudgeResult judge_result;
	static bool judge_result_cleared = false;
	
	static void clear_judge_result() {
		if (judge_result_cleared) return;
		judge_result_cleared = true;
		
		judge_seq_num = -1ul;
		
		stdout_size = 0;
		stderr_size = 0;
		
		memset(&judge_result, 0, sizeof(judge_result));
		judge_result.error = "Not Judged";
	}
	
	void init() {
		LDEBUG_ENTER_RET();
		
		clear_judge_result();
		
		// Stat
		n_judges = 0;
		total_time_ns = 0;
		
		// Buffer
		buffer = Memory::allocate_virtual_memory(INITIAL_BUFFER_SIZE);
		if (!buffer) {
			LFATAL("Allocate buffer failed");
			Utils::GG_reboot();
		}
		buffer_size = INITIAL_BUFFER_SIZE;
		memset(buffer, 0, buffer_size);
	}
	
	// Stat
	
	JudgeStatistics get_statistics() {
		return (JudgeStatistics) {
			.n_judges = n_judges,
			.total_time_ns = total_time_ns,
		};
	}
	
	// Buffer
	
	uint64_t query_buffer_size() {
		return buffer_size;
	}
	
	bool clear_buffer(uint64_t off, uint64_t len) {
		// TODO: no double clear
		if (off < buffer_size && len <= buffer_size - off) {
			memset(buffer + off, 0, len);
			clear_judge_result();
			return true;
		} else {
			return false;
		}
	}
	
	bool read_buffer(uint64_t off, uint64_t len, char *data) {
		if (off < buffer_size && len <= buffer_size - off) {
			memcpy(data, buffer + off, len);
			return true;
		} else {
			return false;
		}
	}
	
	bool write_buffer(uint64_t off, const char *data, uint64_t len) {
		if (off < buffer_size && len <= buffer_size - off) {
			memcpy(buffer + off, data, len);
			clear_judge_result();
			return true;
		} else {
			return false;
		}
	}
	
	bool copy_buffer(uint64_t dst_off, uint64_t src_off, uint64_t len) {
		// TODO: no double copy
		bool dst_in_range = dst_off < buffer_size && len <= buffer_size - dst_off;
		bool src_in_range = src_off < buffer_size && len <= buffer_size - src_off;
		bool no_overlap = dst_off + len <= src_off || src_off + len <= dst_off;
		if (dst_in_range && src_in_range && no_overlap) {
			memcpy(buffer + dst_off, buffer + src_off, len);
			clear_judge_result();
			return true;
		} else {
			return false;
		}
	}
	
	bool compare_buffer(uint64_t off1, uint64_t off2, uint64_t len, bool &result) {
		// TODO: no double compare
		bool in_range_1 = off1 < buffer_size && len <= buffer_size - off1;
		bool in_range_2 = off2 < buffer_size && len <= buffer_size - off2;
		if (in_range_1 && in_range_2) {
			result = memcmp(buffer + off1, buffer + off2, len) == 0;
			return true;
		} else {
			return false;
		}
	}
	
	// Judge
	
	static JudgeResult can_not_load_elf() {
		judge_result.error =  "Can't load ELF";
		return judge_result;
	}
	
	static JudgeResult invalid_time_limit() {
		judge_result.error =  "Invalid time limit";
		return judge_result;
	}
	
	static bool is_valid_buffer(const BufferDesc &b) {
		return b.off < buffer_size && b.len <= buffer_size - b.off;
	}
	
	static bool check_overlap(const BufferDesc &b1, const BufferDesc &b2) {
		return !(b1.off + b1.len <= b2.off || b2.off + b2.len <= b1.off);
	}
	
	static bool check_buffers(const BufferDesc *b, int n) {
		for (int i = 0; i < n; i++) {
			if (!is_valid_buffer(b[i])) return false;
			for (int j = 0; j < i; j++) {
				if (check_overlap(b[i], b[j])) return false;
			}
		}
		return true;
	}
	
	JudgeResult judge(const JudgeRequest &req) {
		if (req.seq_num == judge_seq_num) {
			return judge_result;
		}
		
		// Update stat
		n_judges++;
		
		judge_result_cleared = false;
		judge_seq_num = req.seq_num;
		
		// overflow?
		if (req.memory_hard_limit_kb * 1024 < req.memory_hard_limit_kb) {
			return can_not_load_elf();
		}
		
		BufferDesc buffers[4] = {
			req.ELF, req.stdin, req.stdout, req.stderr,
		};
		
		// invalid or overlapping buffers?
		if (!check_buffers(buffers, sizeof(buffers) / sizeof(buffers[0]))) {
			return can_not_load_elf();
		}
		
		ELF::App app;
		auto elf_conf = (ELF::AppConfig) {
			.memory_hard_limit = req.memory_hard_limit_kb * 1024,
			.stdin_ptr = buffer + req.stdin.off,
			.stdin_size = req.stdin.len,
			.stdout_max_size = req.stdout.len,
			.stderr_max_size = req.stderr.len,
		};
		
		// check time limit
		// TODO: LAPIC limits?
		if (!req.time_limit_ns || req.time_limit_ns > MAX_TIME_LIMIT_NS) {
			return invalid_time_limit();
		}
		
		bool r = ELF::load(buffer + req.ELF.off, req.ELF.len, elf_conf, app);
		if (!r) {
			return can_not_load_elf();
		}
		
		auto res = ELF::run(app, req.time_limit_ns);
		judge_result = (JudgeResult) {
			.error = NULL,
			.count_inst = res.count_inst,
			.clk_thread = res.clk_thread,
			.clk_ref_tsc = res.clk_ref_tsc,
			.time_ns = res.time_ns,
			.time_tsc = res.time_tsc,
			.memory_kb = res.memory_kb,
			.stdout_size = res.stdout_size,
			.stderr_size = res.stderr_size,
			.return_code = res.return_code,
			.is_RE = res.trap_num != 255,  // TRAP_SYSCALL
			.is_TLE = res.trap_num == 32,  // TRAP_IRQ + IRQ_TIMER
		};
		
		if (res.time_ns > req.time_limit_ns || judge_result.is_TLE) {
			judge_result.is_RE = false;
			judge_result.is_TLE = true;
			judge_result.time_ns = req.time_limit_ns;
		}
		
		// Copy stdout and stderr
		// Note: max_size is used instead of actual result size
		memcpy(buffer + req.stdout.off, res.stdout_ptr, req.stdout.len);
		memcpy(buffer + req.stderr.off, res.stderr_ptr, req.stderr.len);
		
		// Update stat
		total_time_ns += judge_result.time_ns;
		
		LINFO("#%lu: time %.6lf ms, mem %lu KiB%s%s",
			n_judges,
			res.time_ns / 1e6, res.memory_kb,
			judge_result.is_RE ? " (RE)" : "",
			judge_result.is_TLE ? " (TLE)" : "");
		
		return judge_result;
	}
}
