#include <inc/memlayout.h>
#include <inc/error.h>
#include <inc/stdio.h>
#include <inc/assert.h>
#include <inc/x86.h>
#include <inc/string.h>

#include <kern/pmem.h>

static pte_t *
pagedir_walk(pde_t *pagedir, void *va, int create)
{
	pde_t *pde = pagedir + PDX(va);
	if (*pde & PTE_P)
		;
	else if (create)
	{
		pte_t *pagetable = (pte_t *) kern_calloc(PGSIZE);
		if (!pagetable)
			return NULL;
		*pde = ((uint32_t) pagetable) | PTE_P | PTE_W | PTE_U;
	}
	else
		return NULL;
	return (pte_t *) PTE_ADDR(*pde) + PTX(va);
}

static int
pagedir_insert(pde_t *pagedir, physaddr_t pa, void *va, int perm)
{
	pte_t *pte = pagedir_walk(pagedir, va, 1);
	if (!pte)
		return -E_NOMEM;
	if (*pte & PTE_P)
		return -E_INVAL;
	*pte = pa | perm | PTE_P;
	return 0;
}

static physaddr_t
pagedir_lookup(pde_t *pagedir, void *va, pte_t **pte_store)
{
	pte_t *pte = pagedir_walk(pagedir, va, 0);
	if (pte_store)
		*pte_store = pte;
	if (!pte || !(*pte & PTE_P))
		return 0;
	return PTE_ADDR(*pte);
}

static void
pagedir_remove(pde_t *pagedir, void *va)
{
	pte_t *pte;
	physaddr_t pa = pagedir_lookup(pagedir, va, &pte);
	if (pa)
		*pte = 0;
}

int
pagedir_insert_region(
	pde_t *pagedir, physaddr_t pa, void *va, size_t bytes, int perm
){
	if (bytes % PGSIZE)
		return -E_INVAL;
	for (size_t i = 0; i < bytes; i += PGSIZE)
	{
		int e = pagedir_insert(pagedir, pa + i, va + i, perm);
		if (e)
		{
			for (size_t j = 0; j < i; j += PGSIZE)
				pagedir_remove(pagedir, va + j);
			return e;
		}
	}
	return 0;
}

static pde_t *kern_pagedir;

void
vmem_init()
{
	cprintf("============ Begin vmem_init() ============\n");
	
	cprintf("Building kern_pagedir......\n");
	kern_pagedir = (pde_t *) kern_alloc(PGSIZE);
	memset(kern_pagedir, 0, PGSIZE);
	int e = pagedir_insert_region(kern_pagedir, 0, 0, -128 << 20, PTE_W);
	assert(!e);
	e = pagedir_insert_region(
		kern_pagedir, 0, (void *) (-128 << 20), 128 << 20, PTE_W
	);
	assert(!e);
	
	cprintf("Installing kern_pagedir......\n");
	lcr3((physaddr_t) kern_pagedir);
	long cr0 = rcr0();
	cr0 |= CR0_PE | CR0_PG | CR0_AM | CR0_WP | CR0_NE | CR0_MP;
	cr0 &= ~(CR0_TS | CR0_EM);
	lcr0(cr0);
	
	cprintf("============ End   vmem_init() ============\n");
}
