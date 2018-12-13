#ifndef JOS_INC_SYSCALL_H
#define JOS_INC_SYSCALL_H

/* system call numbers */
enum {
	SYS_cputs = 0,
	SYS_cgetc,
	SYS_getenvid,
	SYS_env_destroy,
	SYS_page_alloc,
	SYS_page_map,
	SYS_page_unmap,
	SYS_exofork,
	SYS_env_set_status,
	SYS_env_set_trapframe,
	SYS_env_set_pgfault_upcall,
	SYS_yield,
	SYS_ipc_try_send,
	SYS_ipc_recv,
	SYS_time_msec,
	SYS_enter_judge,
	SYS_accept_enter_judge,
	SYS_quit_judge,
	SYS_map_judge_pages,
	SYS_halt,
	SYS_set_tls_base,
	SYS_map_pci_device,
	SYS_page_alloc_with_pa,
	SYS_get_tsc_frequency,
	SYS_send_ipi,
	NSYSCALLS
};

#endif /* !JOS_INC_SYSCALL_H */
