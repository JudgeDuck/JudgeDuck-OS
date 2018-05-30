#ifndef JOS_KERN_PMEM_H
#define JOS_KERN_PMEM_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <kern/proc.h>

/*
	Types to represent a physical and a virtual page, which should be inside
	[0, 4G / 4K).
*/
typedef size_t ppage_t;
typedef size_t vpage_t;

/*
	A `VPageBlock` represents a consecutive virtual memory area, applied and
	owned by a certain process, kernel or idle.
	A `PPageBlock` contains consecutive physical pages, mapped to a consecutive
	virtual pages of a `VPageBlock`.
	A `VPageBlock` needs several `PPageBlock`s, in case of physical memory
	fragmentation. Though we can defrag, but what if the user neither likes it
	nor cares fragmentation? After all, we are making an OS, rather than just
	a judger.
*/
struct PPageBlock;
struct VPageBlock
{
	/*
		Owner of this virtual memory area.
		PID_KERN - the kernel
		PID_NULL - idle
		Others   - an user process
	*/
	pid_t owner;
	/*
		Length (in pages) and starting virtual page.
		If this is the idle block, and `length` is the number of free physical
		pages.
	*/
	size_t length;
	union
	{
		vpage_t vstart;
		struct PPageBlock *ppb_tail;
	};
	/*
		A list of `PPageBlock`s corresponding to this area.
		The elements should be sorted increasingly by virtual address
		`vstart`, unless this is the idle block.
	*/
	struct PPageBlock *ppb_list;
	/*
		Next block of the same process, or next element of the allocator pool. Can be
		`NULL`.
	*/
	struct VPageBlock *next;
};

struct PPageBlock
{
	/* The `VPageBlock` this block belongs to */
	struct VPageBlock *vblock;
	/*
		Next block of the same `vblock`'s `ppb_list`, or next element of
		the allocator pool. Can be `NULL`.
	*/
	struct PPageBlock *next;
	/*
		Length (in pages) and starting physical and virtual page.
		`vstart` is arbitary if `vblock->owner == PID_NULL`.
	*/
	size_t length;
	ppage_t pstart;
	vpage_t vstart;
};

void pmem_init();
void *kern_alloc(size_t bytes);
void *kern_calloc(size_t bytes);
void kern_free(void *a);

#endif	// !JOS_KERN_PMEM_H
