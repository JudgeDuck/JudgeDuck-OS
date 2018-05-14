#ifndef JOS_INC_JUDGE_H
#define JOS_INC_JUDGE_H

struct JudgeParams
{
	// to be implemented, e.g. enable interrupt?
	int ms, kb;
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
	int mem_kb;
};

#endif
