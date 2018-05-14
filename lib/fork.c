// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	//cprintf("pgfault begin utf %p\n", utf);
	//if((uint32_t) utf < 233)
	//{
	//	asm volatile("int $3");
	//}
	
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	
	if(!(uvpd[PDX(addr)] & PTE_P))
	{
		cprintf("not present qwerqwerqwer %p %p %p %p | uvpd %p\n", addr, PDX(addr), uvpd + PDX(addr), uvpd[PDX(addr)], uvpd);
		panic("fork pgfault: not present dir");
	}
	if(!(uvpt[PGNUM(addr)] & PTE_P))
	{
		cprintf("not present %p\n", addr);
		panic("fork pgfault: not present");
	}
	if(!(err & FEC_WR))
		panic("fork pgfault: not a write");
	if(!(uvpt[PGNUM(addr)] & PTE_COW))
	{
		cprintf("sb addr %p eip %p\n", addr, utf->utf_eip);
		panic("fork pgfault: not a copy-on-write page");
	}

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	
	if(sys_page_alloc(0, PFTEMP, PTE_U | PTE_W | PTE_P))
		panic("pgfault: sys_page_alloc failed\n");
	void *begin = (void *) ROUNDDOWN(addr, PGSIZE);
	memcpy(PFTEMP, begin, PGSIZE);
	if(sys_page_map(0, PFTEMP, 0, begin, PTE_U | PTE_W | PTE_P))
		panic("pgfault: sys_page_map failed\n");
	if(sys_page_unmap(0, PFTEMP))
		panic("pgfault: sys_page_unmap failed\n");
	
	//cprintf("pgfault end\n");

	// panic("pgfault not implemented");
}

void
duppage(envid_t dstenv, void *addr)
{
	int r;

	// This is NOT what you should do in your fork.
	if ((r = sys_page_alloc(dstenv, addr, PTE_P|PTE_U|PTE_W)) < 0)
		panic("sys_page_alloc: %e", r);
	if ((r = sys_page_map(dstenv, addr, 0, UTEMP, PTE_P|PTE_U|PTE_W)) < 0)
		panic("sys_page_map: %e", r);
	memmove(UTEMP, addr, PGSIZE);
	if ((r = sys_page_unmap(0, UTEMP)) < 0)
		panic("sys_page_unmap: %e", r);
}

envid_t
fork(void)
{
	envid_t envid;
	uint8_t *addr;
	int r;
	extern unsigned char end[];

	// Allocate a new child environment.
	// The kernel will initialize it with a copy of our register state,
	// so that the child will appear to have called sys_exofork() too -
	// except that in the child, this "fake" call to sys_exofork()
	// will return 0 instead of the envid of the child.
	envid = sys_exofork();
	if (envid < 0)
		panic("sys_exofork: %e", envid);
	if (envid == 0) {
		// We're the child.
		// The copied value of the global variable 'thisenv'
		// is no longer valid (it refers to the parent!).
		// Fix it and return 0.
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}

	// We're the parent.
	// Eagerly copy our entire address space into the child.
	// This is NOT what you should do in your fork implementation.
	for (addr = (uint8_t*) UTEXT; addr < end; addr += PGSIZE)
		duppage(envid, addr);

	// Also copy the stack we are currently running on.
	duppage(envid, ROUNDDOWN(&addr, PGSIZE));

	// Start the child environment running
	if ((r = sys_env_set_status(envid, ENV_RUNNABLE)) < 0)
		panic("sys_env_set_status: %e", r);

	return envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
