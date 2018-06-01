#include <inc/x86.h>
#include <inc/memlayout.h>
#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/stdio.h>
#include <inc/assert.h>
#include <inc/string.h>

#include <kern/pmem.h>
#include <kern/vmem.h>
#include <kern/memdetect.h>

#define BOOT_ALLOC_MAX (4 << 20)

static void *
boot_alloc_impl(size_t bytes)
{
	static void *nextfree;
	if (!nextfree)
	{
		extern char end[];
		nextfree = ROUNDUP((void *) end, PGSIZE);
	}
	void *ret = nextfree;
	nextfree = ROUNDUP(nextfree + bytes, PGSIZE);
	void *top = (void *) (KERNBASE + BOOT_ALLOC_MAX);
	if (nextfree > top)
		panic("boot_alloc: out of memory");
	cprintf("boot_alloc: %p / %p\n", nextfree, top);
	return ret;
}

#define boot_alloc(n, type) (type *) boot_alloc_impl((n) * sizeof(type))

/* Number of physical pages */
static size_t n_ppage;
/* An array of `n_ppage` elements, mapping physical page to the block */
static struct PPageBlock **ppage2ppb;

/* Allocator pool for ppb and vpb */
static struct PPageBlock *ppb_pool;
static size_t n_ppb;
static struct PPageBlock *_ppb_pool_init;
static struct PPageBlock *
ppb_list_take(struct PPageBlock **ppb_list)
{
	if (!*ppb_list)
		return NULL;
	struct PPageBlock *ret = *ppb_list;
	*ppb_list = (*ppb_list)->next;
	return ret;
}
static void
ppb_list_insert(struct PPageBlock **ppb_list, struct PPageBlock *ppb)
{
	ppb->next = *ppb_list;
	*ppb_list = ppb;
}
static struct PPageBlock *
ppb_pool_alloc()
{
	struct PPageBlock *ret = ppb_list_take(&ppb_pool);
	if (!ret)
		panic("ppb_pool_alloc: pool too small");
	return ret;
}
static void
ppb_pool_free(struct PPageBlock *ppb)
{
	ppb_list_insert(&ppb_pool, ppb);
}

static struct VPageBlock *vpb_pool;
static size_t n_vpb;
static struct VPageBlock *_vpb_pool_init;
static struct VPageBlock *
vpb_pool_alloc()
{
	if (!vpb_pool)
		panic("vpb_alloc: pool too small");
	struct VPageBlock *ret = vpb_pool;
	vpb_pool = vpb_pool->next;
	return ret;
}
static void
vpb_pool_free(struct VPageBlock *vpb)
{
	vpb->next = vpb_pool;
	vpb_pool = vpb;
}

/* Idle and kernel vpb */
static struct VPageBlock *vpb_idle;
static struct VPageBlock *kern_vpb_list;

static struct PPageBlock *
vpb_idle_take_front()
{
	struct PPageBlock *ppb = ppb_list_take(&vpb_idle->ppb_list);
	if (ppb == vpb_idle->ppb_tail)
		vpb_idle->ppb_tail = NULL;
	vpb_idle->length -= ppb->length;
	return ppb;
}
static void
vpb_idle_push_front(struct PPageBlock *ppb)
{
	vpb_idle->length += ppb->length;
	ppb_list_insert(&vpb_idle->ppb_list, ppb);
	if (!vpb_idle->ppb_tail)
		vpb_idle->ppb_tail = ppb;
}
static void
vpb_idle_push_back(struct PPageBlock *ppb)
{
	vpb_idle->length += ppb->length;
	if (vpb_idle->ppb_tail)
	{
		assert(!vpb_idle->ppb_tail->next);
		vpb_idle->ppb_tail->next = ppb;
		ppb->next = NULL;
	}
	else
	{
		vpb_idle->ppb_list = ppb;
		vpb_idle->ppb_tail = ppb;
	}
}

/*
	Allocate `length` consecutive physical pages. The caller should properly
	set the `vblock`, `next` and `vstart` fields of return value.
*/
static struct PPageBlock *
ppb_alloc(size_t length)
{
	struct PPageBlock *start_ppb = NULL;
	while (1)
	{
		if (vpb_idle->ppb_list == start_ppb)
			return NULL;
		if (!start_ppb)
			start_ppb = vpb_idle->ppb_list;
		struct PPageBlock *ppb = vpb_idle_take_front();
		if (ppb->length >= length)
		{
			struct PPageBlock *ret = ppb_pool_alloc();
			ret->length = length;
			ret->pstart = ppb->pstart;
			ppb->length -= length;
			ppb->pstart += length;
			if (ppb->length)
				vpb_idle_push_front(ppb);
			else
				ppb_pool_free(ppb);
			for (size_t i = 0; i < ret->length; ++i)
				ppage2ppb[ret->pstart + i] = ret;
			return ret;
		}
		vpb_idle_push_back(ppb);
	}
}

static struct VPageBlock *
vpb_alloc(pid_t owner, vpage_t vstart, size_t length, int consecutive)
{
	if (consecutive)
	{
		struct PPageBlock *ppb = ppb_alloc(length);
		if (!ppb)
			return NULL;
		ppb->vstart = vstart;
		ppb->next = NULL;
		struct VPageBlock *vpb = vpb_pool_alloc(); // always success
		vpb->owner = owner;
		vpb->length = length;
		vpb->vstart = vstart;
		vpb->ppb_list = ppb;
		ppb->vblock = vpb;
		return vpb;
	}
	else
	{
		panic("Not implemented yet!");
	}
}

static void
vpb_remap(struct VPageBlock *vpb, vpage_t vstart)
{
	vpb->vstart = vstart;
	vpage_t cur = vpb->vstart;
	for (struct PPageBlock *ppb = vpb->ppb_list; ppb; ppb = ppb->next)
	{
		ppb->vstart = cur;
		cur += ppb->length;
	}
	assert(cur == vpb->vstart + vpb->length);
}

void *
kern_alloc(size_t bytes)
{
	bytes = ROUNDUP(bytes, PGSIZE);
	struct VPageBlock *vpb = vpb_alloc(PID_KERN, 0, bytes >> PGSHIFT, 1);
	assert(vpb);
	assert(vpb->ppb_list);
	assert(!vpb->ppb_list->next);
	vpb_remap(vpb, vpb->ppb_list->pstart);
	return (void *) (vpb->vstart << PGSHIFT);
}

void *
kern_calloc(size_t bytes)
{
	void *a = kern_alloc(bytes);
	if (a)
		memset(a, 0, bytes);
	return a;
}

void
kern_free(void *a)
{
	warn("kern_free not implemented\n");
}

static void
ppb_print(struct PPageBlock *ppb)
{
	cprintf("  ppb id %d from vpb %d\n", ppb - _ppb_pool_init,
		ppb->vblock - _vpb_pool_init);
	if (ppb->vblock && ppb->vblock->owner == PID_NULL)
		cprintf("  pa[%p, %p] IDLE\n", ppb->pstart << PGSHIFT,
			((ppb->pstart + ppb->length) << PGSHIFT) - 1);
	else
		cprintf("  pa[%p, %p] va[%p, %p]\n", ppb->pstart << PGSHIFT,
			((ppb->pstart + ppb->length) << PGSHIFT) - 1,
			ppb->vstart << PGSHIFT,
			((ppb->vstart + ppb->length) << PGSHIFT) - 1);
}

static void
vpb_print(struct VPageBlock *vpb)
{
	cprintf(" vpb %d ", vpb - _vpb_pool_init);
	if (vpb->owner == PID_KERN)
		cprintf("from KERNEL\n");
	else if (vpb->owner == PID_NULL)
		cprintf("IDLE\n");
	else
		cprintf("from pid %p\n", vpb->owner);
	
	if (vpb->owner == PID_NULL)
		cprintf(" free %d pages, %d KB\n", vpb->length,
			vpb->length * PGSIZE / 1024);
	else
		cprintf(" va[%p, %p]\n", vpb->vstart << PGSHIFT,
			((vpb->vstart + vpb->length) << PGSHIFT) - 1);
	for (struct PPageBlock *ppb = vpb->ppb_list; ppb; ppb = ppb->next)
		ppb_print(ppb);
}

void
pmem_init()
{
	cprintf("============ Begin pmem_init() ============\n");
	
	/* Get `n_ppage` */
	n_ppage = i386_detect_memory();
	
	/* Build pools */
	cprintf("Building pools......\n");
	
	n_ppb = n_ppage; // So pessimistic?
	ppb_pool = _ppb_pool_init = boot_alloc(n_ppb, struct PPageBlock);
	for (size_t i = 1; i < n_ppb; ++i)
		ppb_pool[i - 1].next = ppb_pool + i;
	
	n_vpb = n_ppage; // So pessimistic?
	vpb_pool = _vpb_pool_init = boot_alloc(n_vpb, struct VPageBlock);
	for (size_t i = 1; i < n_vpb; ++i)
		vpb_pool[i - 1].next = vpb_pool + i;
	
	/* Build initial `vpb_idle` */
	cprintf("Building vpb_idle.....\n");
	vpb_idle = vpb_pool_alloc();
	vpb_idle->owner = PID_NULL;
	vpb_idle->length = n_ppage;
	vpb_idle->ppb_list = ppb_pool_alloc();
	vpb_idle->ppb_tail = vpb_idle->ppb_list;
	vpb_idle->ppb_list->vblock = vpb_idle;
	vpb_idle->ppb_list->next = NULL;
	vpb_idle->ppb_list->length = n_ppage;
	vpb_idle->ppb_list->pstart = 0;
	
	/* Build initial `ppage2ppb` */
	cprintf("Building ppage2ppb.....\n");
	ppage2ppb = boot_alloc(n_ppage, struct PPageBlock *); 
	for (size_t i = 0; i < n_ppage; ++i)
		ppage2ppb[i] = vpb_idle->ppb_list;
	
	/* Preserve space for kernel */
	// cprintf("------------ Before Alloc ------------\n");
	// vpb_print(vpb_idle);
	struct VPageBlock *vpb = vpb_alloc(PID_KERN, KERNBASE >> PGSHIFT,
		BOOT_ALLOC_MAX >> PGSHIFT, 1);
	// cprintf("------------ After  Alloc ------------\n");
	// vpb_print(vpb_idle);
	// vpb_print(vpb);
	
	cprintf("============ End   pmem_init() ============\n");
	
	char *str = (char *) kern_alloc(233);
	cprintf("str = %p\n", str);
	for (int i = 0; i < 48; ++i)
		str[i] = 32 + i;
	str[48] = '\n';
	for (int i = 0; i < 48; ++i)
		str[i + 49] = 80 + i;
	str[97] = '\n';
	str[98] = 0;
	cprintf(str);
}
