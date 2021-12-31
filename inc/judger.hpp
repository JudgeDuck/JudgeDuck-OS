#ifndef DUCK_JUDGER_H
#define DUCK_JUDGER_H

#include <stdint.h>

namespace Judger {
	struct JudgeStatistics {
		uint64_t n_judges;
		uint64_t total_time_ns;
	};
	
	struct BufferDesc {
		uint64_t off, len;
	};
	
	struct JudgeRequest {
		uint64_t seq_num;
		uint64_t time_limit_ns;
		uint64_t memory_hard_limit_kb;
		BufferDesc ELF;
		BufferDesc stdin;  // copy from buffer
		BufferDesc stdout, stderr;  // copy to buffer
		BufferDesc IB, OB;  // copy from and to
		uint64_t OB_need_clear;
	};
	
	struct JudgeResult {
		const char *error;
		uint64_t count_inst;  // INST_RETIRED_ANY
		uint64_t clk_thread;  // CPU_CLK_UNHALTED_THREAD
		uint64_t clk_ref_tsc;  // CPU_CLK_UNHALTED_REF_TSC
		uint64_t time_ns;
		uint64_t time_ns_ref_tsc;
		uint64_t time_ns_real;
		uint64_t time_tsc;
		uint64_t memory_kb;  // dirty pages
		uint64_t memory_kb_accessed;  // accessed pages (without stdin)
		uint64_t stdout_size;
		uint64_t stderr_size;
		int32_t return_code;
		uint8_t trap_num;
		uint64_t trap_epc;
		uint64_t trap_cr2;
		bool is_RE;  // Runtime Error
		bool is_TLE;  // Time Limit Exceeded
	};
	
	void init();
	
	// Stat
	JudgeStatistics get_statistics();
	
	// Buffer (atomic operations)
	uint64_t query_buffer_size();
	bool clear_buffer(uint64_t off, uint64_t len);
	bool read_buffer(uint64_t off, uint64_t len, char *data);
	bool write_buffer(uint64_t off, const char *data, uint64_t len);
	bool copy_buffer(uint64_t dst_off, uint64_t src_off, uint64_t len);  // no overlap
	bool compare_buffer(uint64_t off1, uint64_t off2, uint64_t len, bool &result);
	
	// Cache
	bool load_cache(const char *cache_name, uint64_t dst_off, uint64_t dst_len, const char *hex);
	bool store_cache(const char *cache_name, uint64_t src_off, uint64_t src_len, const char *hex);
	void get_cache_info(const char *cache_name, char *output);
	
	// Judge
	JudgeResult judge(const JudgeRequest &req);
}

#endif
