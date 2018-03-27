#ifndef JOS_KERN_PROC_H
#define JOS_KERN_PROC_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

typedef size_t pid_t;

#define PID_NULL ((pid_t) 0)
#define PID_KERN ((pid_t) 1)
#define PID_USER_BASE ((pid_t) 2)

#define MAX_PROCS 1024

struct VPageBlock;
struct Proc
{
	pid_t pid;
	pid_t parent_pid;
	/*
		List of `VPageBlock`s of this process, sorted increasingly by virtual
		address. Can be `NULL`.
	*/
	struct VPageBlock *vpb_list;
	unsigned is_runnable: 1;
	unsigned is_running: 1;
	/*
		`is_critical` means whether this is currently running as a critical
		process.
		`is_privileged` means whether this process has the right to let itself
		or children run as critical process, or to grant this privilege to
		another process.
	*/
	unsigned is_critical: 1;
	unsigned is_privileged: 1;
};

extern struct Proc *procs;

#endif	// !JOS_KERN_PROC_H
