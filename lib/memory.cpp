#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <inc/memory.hpp>
#include <inc/logger.hpp>

extern int ebss;

namespace Memory {
	const uint64_t MAX_MEMORY_SIZE = 511ull << 30;  // 511GiB
	const uint32_t MAX_N_HUGE_PAGES = MAX_MEMORY_SIZE / HUGE_PAGE_SIZE;  // 261632
	
	static bool huge_page_map[MAX_N_HUGE_PAGES];
	static int n_huge_pages = 0;
	
	void init() {
		LDEBUG_ENTER_RET();
		printf("Kernel memory used: %.1lf MiB\n", (uint64_t) &ebss / 1048576.0);
		assert((uint64_t) &ebss <= 4u << 20);  // Check 4 MiB
		printf("n_huge_pages = %d\n", n_huge_pages);
	}
	
	void register_available_huge_page(void *addr_ptr) {
		uint64_t addr = (uint64_t) addr_ptr;
		assert(addr % HUGE_PAGE_SIZE == 0);
		assert(addr < MAX_MEMORY_SIZE);
		
		uint32_t page_num = addr / HUGE_PAGE_SIZE;
		if (!huge_page_map[page_num]) {
			huge_page_map[page_num] = true;
			++n_huge_pages;
		}
	}
}
