#include <inc/memlayout.h>
#include <inc/pmem.h>

static pte_t *
pagedir_walk(pde_t *pagedir, void *va, int create)
{
	pde_t *pde = pagedir + PDX(va);
	if (*pde & PTE_P)
		;
	else if (create)
	{
		pte_t *pagetable = kern_calloc(PGSIZE);
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

struct physaddr_t
pagedir_lookup(pde_t *pagedir, void *va, pte_t **pte_store)
{
	pte_t *pte = pagedir_walk(pagedir, va, 0);
	if (pte_store)
		*pte_store = pte;
	if (!pte || !(*pte & PTE_P))
		return NULL;
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
