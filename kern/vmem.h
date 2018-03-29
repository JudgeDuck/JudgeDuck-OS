#include <inc/memlayout.h>

int pagedir_insert_region(
	pde_t *pagedir, physaddr_t pa, void *va, size_t bytes, int perm
);
void vmem_init();
