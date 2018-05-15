#ifndef JOS_INC_JUDGE_H
#define JOS_INC_JUDGE_H

#include <inc/syscall.h>

struct JudgeParams
{
	int ms, kb;
	int syscall_enabled[NSYSCALLS];
	// writable_addr_begin and (writable_addr_end + 1) must be page-aligned
	void *writable_addr_begin, *writable_addr_end;
	bool defrag_mem; // to be implemented
};

enum JudgeVerdict
{
	VERDICT_OK,
	VERDICT_TLE, // Time Limit Exceeded
	VERDICT_RE,  // Runtime Error
	VERDICT_IS,  // Illegal Syscall
	VERDICT_SE,  // System Error
};

struct JudgeResult
{
	enum JudgeVerdict verdict;
	uint64_t time_cycles;
	uint64_t time_ns;
	int syscall_id;
	int mem_kb; // to be implemented
};

#endif
