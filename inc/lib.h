// Main public header file for our user-land support library,
// whose code lives in the lib directory.
// This library is roughly our OS's version of a standard C library,
// and is intended to be linked into all user-mode applications
// (NOT the kernel or boot loader).

#ifndef JOS_INC_LIB_H
#define JOS_INC_LIB_H 1

#include <inc/types.h>
#include <inc/stdio.h>
#include <inc/stdarg.h>
#include <inc/string.h>
#include <inc/error.h>
#include <inc/assert.h>
#include <inc/env.h>
#include <inc/memlayout.h>
#include <inc/syscall.h>
#include <inc/trap.h>
#include <inc/judge.h>
#include <inc/fs.h>
#include <inc/fd.h>
#include <inc/args.h>
#include <inc/malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USED(x)		(void)(x)

// main user program
void	umain(int argc, char **argv);

// libmain.c or entry.S
extern const char *binaryname;
extern const volatile struct Env *thisenv;
extern const volatile struct Env envs[NENV];
extern const volatile struct PageInfo pages[];

// exit.c
void	exit(void);

// pgfault.c
void	set_pgfault_handler(void (*handler)(struct UTrapframe *utf));

// readline.c
char*	readline(const char *buf);

// syscall.c
void	sys_cputs(const char *string, size_t len);
int	sys_cgetc(void);
envid_t	sys_getenvid(void);
int	sys_env_destroy(envid_t);
void	sys_yield(void);
void	sys_halt(void);
static envid_t sys_exofork(void);
static envid_t sys_exofork1(int);
int	sys_env_set_status(envid_t env, int status);
int	sys_env_set_trapframe(envid_t env, struct Trapframe *tf);
int	sys_env_set_pgfault_upcall(envid_t env, void *upcall);
int	sys_page_alloc(envid_t env, void *pg, int perm);
int	sys_page_map(envid_t src_env, void *src_pg,
		     envid_t dst_env, void *dst_pg, int perm);
int	sys_page_unmap(envid_t env, void *pg);
int	sys_ipc_try_send(envid_t to_env, uint32_t value, void *pg, int perm);
int	sys_ipc_recv(void *rcv_pg);
int sys_enter_judge(void *eip, struct JudgeParams *prm);
int sys_accept_enter_judge(envid_t envid, struct JudgeResult *res);
int sys_quit_judge();
int sys_map_judge_pages(void *dst, unsigned offset, unsigned len);  // returns: how many pages actually mapped
unsigned int sys_time_msec(void);
int sys_set_tls_base(void *base);
int sys_send_ipi(int vector);

// PCI
// TODO: lspci
int sys_map_pci_device(uint32_t key1, uint32_t key2, void *base, int maxlen);  // returns: how many bytes actually mapped
int sys_page_alloc_with_pa(envid_t env, void *pg, int perm, uint32_t *pa_store);

// Time
int sys_get_tsc_frequency(uint64_t *freq_store);

// This must be inlined.  Exercise for reader: why?
static inline envid_t __attribute__((always_inline))
sys_exofork(void)
{
	int a1 = 0;
	envid_t ret;
	asm volatile("int %2"
		     : "=a" (ret)
		     : "a" (SYS_exofork), "i" (T_SYSCALL), "d" (a1));
	return ret;
}

static inline envid_t __attribute__((always_inline))
sys_exofork1(int a1)
{
	envid_t ret;
	asm volatile("int %2"
		     : "=a" (ret)
		     : "a" (SYS_exofork), "i" (T_SYSCALL), "d" (a1));
	return ret;
}

// ipc.c
void	ipc_send(envid_t to_env, uint32_t value, void *pg, int perm);
int32_t ipc_recv(envid_t *from_env_store, void *pg, int *perm_store);
envid_t	ipc_find_env(enum EnvType type);

// fork.c
#define	PTE_SHARE	0x400
envid_t	fork(void);
envid_t	sfork(void);	// Challenge!

// fd.c
int	close(int fd);
ssize_t	read(int fd, void *buf, size_t nbytes);
ssize_t	write(int fd, const void *buf, size_t nbytes);
int	seek(int fd, off_t offset);
void	close_all(void);
ssize_t	readn(int fd, void *buf, size_t nbytes);
int	dup(int oldfd, int newfd);
int	fstat(int fd, struct Stat *statbuf);
int	stat(const char *path, struct Stat *statbuf);

// file.c
int	open(const char *path, int mode);
int	ftruncate(int fd, off_t size);
int	remove(const char *path);
int	sync(void);

// pageref.c
int	pageref(void *addr);

// spawn.c
envid_t	spawn(const char *program, const char **argv);
envid_t	spawn_contestant(const char *program, const char **argv);
envid_t	spawnl(const char *program, const char *arg0, ...);
envid_t	spawn_from_memory(const char *program, const char **argv);
envid_t	spawn_contestant_from_memory(const char *program, const char **argv);
envid_t	spawnl_from_memory(const char *program, const char *arg0, ...);


// console.c
void	cputchar(int c);
int	getchar(void);
int	iscons(int fd);
int	opencons(void);

// pipe.c
int	pipe(int pipefds[2]);
int	pipeisclosed(int pipefd);

// wait.c
void	wait(envid_t env);

/* File open modes */
#define	O_RDONLY	0x0000		/* open for reading only */
#define	O_WRONLY	0x0001		/* open for writing only */
#define	O_RDWR		0x0002		/* open for reading and writing */
#define	O_ACCMODE	0x0003		/* mask for above modes */

#define	O_CREAT		0x0100		/* create if nonexistent */
#define	O_TRUNC		0x0200		/* truncate to zero length */
#define	O_EXCL		0x0400		/* error if already exists */
#define O_MKDIR		0x0800		/* create directory, not regular file */

#ifdef __cplusplus
}
#endif

#endif	// !JOS_INC_LIB_H
