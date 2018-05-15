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

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?) // TODO: fuck it
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	
	void *addr = (void *) (pn * PGSIZE);
	
	uint32_t perm = uvpt[PGNUM(addr)] & PTE_SYSCALL;
	if((perm & PTE_W) && !(perm & PTE_SHARE))
		perm = (perm & ~PTE_W) | PTE_COW;
	//cprintf("map %d\n", pn);
	if(sys_page_map(0, addr, envid, addr, perm))
		panic("duppage: map error");
	if(sys_page_map(0, addr, 0, addr, perm))
		panic("duppage: remap error");
	
	// panic("duppage not implemented");
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	//cprintf("addr thisenv %p\n", &thisenv);
	set_pgfault_handler(pgfault);
	envid_t child = sys_exofork();
	if(child)
	{
		for(unsigned i = 0; i < UXSTACKTOP / PGSIZE - 1; i++)
		{
			//cprintf("fuck %d\n", i);
			if((uvpd[i >> 10] & PTE_P) && (uvpt[i] & PTE_P))
			{
				if(duppage(child, i))
					panic("fork: duppage gg");
				//cprintf("duppaged %x\n", i);
			}
		}
		if(sys_page_alloc(child, (void *) (UXSTACKTOP - PGSIZE), PTE_U | PTE_W | PTE_P))
			panic("fork: alloc gg");
		extern void _pgfault_upcall(void);
		//extern void (*_pgfault_handler)(struct UTrapframe *utf);
		//if(sys_page_map(child, (void *) (PGNUM(&thisenv) << PGSHIFT), thisenv->env_id, (void *) UTEMP, PTE_U | PTE_W | PTE_P))
		//	panic("fork: asdf map gg");
		//*(const volatile struct Env **) (UTEMP + PGOFF(thisenv)) = mogic;
		//if(sys_page_unmap(thisenv->env_id, (void *) UTEMP))
		//	panic("fork: asdf unmap gg");
		if(sys_env_set_pgfault_upcall(child, _pgfault_upcall))
			panic("fork: set gg");
		if(sys_env_set_status(child, ENV_RUNNABLE))
			panic("fork: status gg");
		return child;
	}
	else
	{
		//thisenv = mogic;
		thisenv = envs + ENVX(sys_getenvid());
		//cprintf("======================================\nchild envid %x\n", thisenv->env_id);
		set_pgfault_handler(pgfault);
		//_pgfault_handler = 0;
		return 0;
	}
	// panic("fork not implemented");
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
