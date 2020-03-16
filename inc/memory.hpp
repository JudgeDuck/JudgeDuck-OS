#ifndef DUCK_MEMORY_H
#define DUCK_MEMORY_H

namespace Memory {
	const uint32_t PAGE_SIZE = 4096;  // 4KiB
	const uint32_t HUGE_PAGE_SIZE = 2097152;  // 2MiB
	
	void init();
	
	void register_available_huge_page(void *addr);
}

#endif
