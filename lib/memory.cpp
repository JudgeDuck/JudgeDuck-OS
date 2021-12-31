#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <inc/memory.hpp>
#include <inc/logger.hpp>
#include <inc/utils.hpp>
#include <inc/x86_64.hpp>

extern int ebss;

namespace Memory {
	const uint64_t MAX_MEMORY_SIZE = 511ull << 30;  // 511GiB
	const uint64_t MAX_N_HUGE_PAGES = MAX_MEMORY_SIZE / HUGE_PAGE_SIZE;  // 261632
	
	static bool huge_page_map[MAX_N_HUGE_PAGES];
	static uint64_t n_huge_pages = 0;
	
	const uint64_t kernel_break = 4 << 20;  // 4 MiB
	
	const uint64_t PTE_PRESENT = 1 << 0;
	const uint64_t PTE_WRITABLE = 1 << 1;
	const uint64_t PTE_USER = 1 << 2;
	const uint64_t PTE_WRITE_THROUGH = 1 << 3;
	const uint64_t PTE_CACHE_DISABLE = 1 << 4;
	const uint64_t PTE_ACCESSED = 1 << 5;
	const uint64_t PTE_DIRTY = 1 << 6;
	const uint64_t PTE_HUGE = 1 << 7;
	const uint64_t PTE_GLOBAL = 1 << 8;
	const uint64_t PTE_NO_EXECUTE = 1ull << 63;
	
	// OS-available flags
	const uint64_t PTE_DUCK_WRITTEN = 1 << 9;
	
	static inline uint64_t clear_page_flags(uint64_t a) {
		// return ((a << 1) >> 13) << 12;
		
		// BIT 9, 10, 11 are available for OS
		return ((a << 1) >> 10) << 9;
	}
	
	// For the page table allocator
	static uint64_t next_page_table_address = kernel_break;
	static uint64_t page_table_break;
	static uint64_t vaddr_break;
	
	// Allocate a 4 KiB-sized (512 entries) page table
	static uint64_t page_table_alloc() {
		// find next available huge page
		while (!huge_page_map[next_page_table_address / HUGE_PAGE_SIZE]) {
			next_page_table_address += HUGE_PAGE_SIZE;
		}
		
		uint64_t ret = next_page_table_address;
		next_page_table_address += PAGE_SIZE;
		assert(next_page_table_address <= page_table_break);
		return ret;
	}
	
	static uint64_t page_table_alloc_zeroed() {
		uint64_t ret = page_table_alloc();
		memset((void *) remap(ret), 0, PAGE_SIZE);
		return ret;
	}
	
	// Returns the remapped next-level PTE
	static inline uint64_t & PTE(uint64_t page_table_paddr, uint64_t index) {
		return * (uint64_t *) remap(page_table_paddr | (index * 8));
	}
	
	static inline uint64_t & PTE_create(uint64_t page_table_paddr, uint64_t index,
		uint64_t creation_flags) {
		uint64_t &ret = PTE(page_table_paddr, index);
		if (!ret) ret = page_table_alloc_zeroed() | creation_flags;
		return ret;
	}
	
	static inline uint64_t P4_index(uint64_t vaddr) {
		return (vaddr >> 39) & (PAGE_SIZE / 8 - 1);
	}
	
	static inline uint64_t P3_index(uint64_t vaddr) {
		return (vaddr >> 30) & (PAGE_SIZE / 8 - 1);
	}
	
	static inline uint64_t P2_index(uint64_t vaddr) {
		return (vaddr >> 21) & (PAGE_SIZE / 8 - 1);
	}
	
	static inline uint64_t P1_index(uint64_t vaddr) {
		return (vaddr >> 12) & (PAGE_SIZE / 8 - 1);
	}
	
	// Get recursively mapped first level PTE
	static inline uint64_t & get_P1(uint64_t vaddr) {
		return * (uint64_t *) (((vaddr >> 9) & -8) | -(1ull << 39));
	}
	
	// Get recursively mapped second level PTE
	static inline uint64_t & get_P2(uint64_t vaddr) {
		return * (uint64_t *) (((vaddr >> 18) & -8) | -(1ull << 30));
	}
	
	// Get recursively mapped third level PTE
	static inline uint64_t & get_P3(uint64_t vaddr) {
		return * (uint64_t *) (((vaddr >> 27) & -8) | -(1ull << 21));
	}
	
	// Get recursively mapped fourth level PTE
	static inline uint64_t & get_P4(uint64_t vaddr) {
		return * (uint64_t *) (((vaddr >> 36) & -8) | -(1ull << 12));
	}
	
	static void init_page_table_break() {
		LDEBUG_ENTER_RET();
		
		uint64_t page_table_size = PAGE_SIZE * 4;  // reserved
		page_table_size += n_huge_pages * PAGE_SIZE;  // 4k P1
		page_table_size += Utils::round_up(n_huge_pages * 8, PAGE_SIZE);  // 4k P2
		page_table_size += PAGE_SIZE * 2;  // 4k P3 and P4
		
		page_table_size = Utils::round_up(page_table_size, HUGE_PAGE_SIZE);
		uint64_t cur_size = 0;
		for (uint64_t i = 0; i < n_huge_pages; i++) {
			if (!huge_page_map[i]) continue;
			cur_size += HUGE_PAGE_SIZE;
			if (cur_size >= page_table_size) {
				page_table_break = (i + 1) * HUGE_PAGE_SIZE;
				break;
			}
		}
		
		LINFO("page_table_break = %x (%.1lf MiB)",
			page_table_break, page_table_break / 1048576.0);
	}
	
	static uint64_t init_empty_kernel_page_table() {
		LDEBUG_ENTER_RET();
		
		uint64_t P4 = page_table_alloc_zeroed();  // 512G pages
		uint64_t P3_high = page_table_alloc_zeroed();  // 1G pages for remapping
		uint64_t P3_low = page_table_alloc_zeroed();  // 1G pages in positive address
		uint64_t P2_low = page_table_alloc_zeroed();  // 2M pages in positive address
		
		// Set up recursive mapping [-512 GiB, 0)
		PTE(P4, 511) = P4 | PTE_PRESENT | PTE_WRITABLE | PTE_DIRTY | PTE_ACCESSED;
		
		// Set up physcial memory remap [-1024 GiB, -512 GiB)
		PTE(P4, 510) = P3_high | PTE_PRESENT | PTE_WRITABLE | PTE_DIRTY | PTE_ACCESSED;
		for (uint64_t i = 0; i < PAGE_SIZE / 8; i++) {
			PTE(P3_high, i) = (i * P3_PAGE_SIZE)
				| PTE_PRESENT | PTE_WRITABLE | PTE_DIRTY | PTE_ACCESSED | PTE_HUGE;
		}
		
		// Set up kernel mapping [0, 4 MiB)
		PTE(P4, 0) = P3_low | PTE_PRESENT | PTE_WRITABLE | PTE_DIRTY | PTE_ACCESSED | PTE_USER;
		PTE(P3_low, 0) = P2_low | PTE_PRESENT | PTE_WRITABLE | PTE_DIRTY | PTE_ACCESSED | PTE_USER;
		assert(kernel_break % HUGE_PAGE_SIZE == 0);
		for (uint64_t i = 0; i < kernel_break / HUGE_PAGE_SIZE; i++) {
			uint64_t P1 = page_table_alloc_zeroed();
			PTE(P2_low, i) = P1 | PTE_PRESENT | PTE_WRITABLE | PTE_DIRTY | PTE_ACCESSED;
			
			// Set up 4 KiB pages in one huge page
			for (uint64_t j = 0; j < PAGE_SIZE / 8; j++) {
				PTE(P1, j) = (i * HUGE_PAGE_SIZE + j * PAGE_SIZE)
					| PTE_PRESENT | PTE_WRITABLE | PTE_DIRTY | PTE_ACCESSED;
			}
		}
		
		return P4;
	}
	
	static uint64_t init_page_table_4k() {
		LDEBUG_ENTER_RET();
		
		uint64_t P4 = init_empty_kernel_page_table();
		
		// Map user pages
		uint64_t vaddr = kernel_break;
		for (uint64_t i = page_table_break / HUGE_PAGE_SIZE; i < MAX_N_HUGE_PAGES; i++) {
			if (!huge_page_map[i]) continue;
			uint64_t paddr = i * HUGE_PAGE_SIZE;
			
			uint64_t flags = PTE_PRESENT | PTE_WRITABLE | PTE_USER | PTE_ACCESSED | PTE_DIRTY;
			uint64_t P3 = clear_page_flags(PTE_create(P4, P4_index(vaddr), flags));
			uint64_t P2 = clear_page_flags(PTE_create(P3, P3_index(vaddr), flags));
			uint64_t P1 = clear_page_flags(PTE_create(P2, P2_index(vaddr), flags));  // 4K pages
			
			for (uint64_t j = 0; j < PAGE_SIZE / 8; j++) {
				PTE(P1, j) = (paddr + j * PAGE_SIZE) | flags;
				PTE(P1, j) &= ~PTE_USER;
				PTE(P1, j) |= PTE_DUCK_WRITTEN;  // The content of the page will be cleared later
			}
			
			vaddr += HUGE_PAGE_SIZE;
		}
		
		vaddr_break = vaddr;
		
		return P4;
	}
	
	static void init_page_tables() {
		LDEBUG_ENTER_RET();
		init_page_table_break();
		uint64_t P4 = init_page_table_4k();
		LINFO("vaddr_break = %x (%.1lf MiB)", vaddr_break, vaddr_break / 1048576.0);
		x86_64::lcr3(P4);
	}
	
	void init() {
		LDEBUG_ENTER_RET();
		LINFO("Kernel memory used: %.1lf MiB", (uint64_t) &ebss / 1048576.0);
		assert((uint64_t) &ebss <= kernel_break);
		LDEBUG("n_huge_pages = %d", n_huge_pages);
		
		init_page_tables();
	}
	
	void register_available_huge_page(void *addr_ptr) {
		uint64_t addr = (uint64_t) addr_ptr;
		assert(addr % HUGE_PAGE_SIZE == 0);
		assert(addr < MAX_MEMORY_SIZE);
		
		if (addr < kernel_break) return;
		
		uint32_t page_num = addr / HUGE_PAGE_SIZE;
		if (!huge_page_map[page_num]) {
			huge_page_map[page_num] = true;
			++n_huge_pages;
		}
	}
	
	uint64_t get_kernel_break() {
		return kernel_break;
	}
	
	uint64_t get_vaddr_break() {
		return vaddr_break;
	}
	
	// TODO: Support huge paging
	void set_page_flags_user_writable(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			P1 = clear_page_flags(P1)
				| PTE_PRESENT | PTE_USER | PTE_WRITABLE | PTE_NO_EXECUTE;
			
			start += PAGE_SIZE;
		}
	}
	
	// TODO: Support huge paging
	void set_page_flags_user_readonly(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			P1 = clear_page_flags(P1) | PTE_PRESENT | PTE_USER | PTE_NO_EXECUTE;
			
			start += PAGE_SIZE;
		}
	}
	
	// TODO: Support huge paging
	void set_page_flags_user_executable(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			P1 = clear_page_flags(P1) | PTE_PRESENT | PTE_USER;
			
			start += PAGE_SIZE;
		}
	}
	
	// TODO: Support huge paging
	void set_page_flags_kernel(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			P1 = clear_page_flags(P1) | PTE_PRESENT | PTE_WRITABLE;
			
			start += PAGE_SIZE;
		}
	}
	
	// TODO: Support huge paging
	void add_page_flags_writable(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			P1 |= PTE_WRITABLE;
			
			start += PAGE_SIZE;
		}
	}
	
	// TODO: Support huge paging
	void add_page_flags_executable(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			P1 &= ~PTE_NO_EXECUTE;
			
			start += PAGE_SIZE;
		}
	}
	
	// TODO: Support huge paging
	void clear_access_and_dirty_flags(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			P1 &= ~(PTE_ACCESSED | PTE_DIRTY);
			
			start += PAGE_SIZE;
		}
	}
	
	// TODO: Support huge paging
	uint64_t count_dirty_pages(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		uint64_t ret = 0;
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			ret += (P1 & PTE_DIRTY) ? 1 : 0;
			
			start += PAGE_SIZE;
		}
		return ret;
	}
	
	uint64_t count_accessed_pages(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		uint64_t ret = 0;
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			ret += (P1 & PTE_ACCESSED) ? 1 : 0;
			
			start += PAGE_SIZE;
		}
		return ret;
	}
	
	void set_duck_written_by_dirty(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			bool has_dirty = (P1 & PTE_DIRTY);
			P1 |= has_dirty ? PTE_DUCK_WRITTEN : 0;
			
			start += PAGE_SIZE;
		}
	}
	
	void set_duck_written(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			P1 |= PTE_DUCK_WRITTEN;
			
			start += PAGE_SIZE;
		}
	}
	
	void clear_duck_written_pages(uint64_t start, uint64_t end) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			if ((P1 & PTE_DUCK_WRITTEN)) {
				memset((void *) start, 0, PAGE_SIZE);
				P1 &= ~(PTE_DIRTY | PTE_DUCK_WRITTEN);
			}
			
			start += PAGE_SIZE;
		}
	}
	
	// Note: 4k-paged
	void map_region_cache_disabled(uint64_t start, uint64_t end, uint64_t src_addr) {
		assert(start % PAGE_SIZE == 0);
		assert(end % PAGE_SIZE == 0);
		assert(src_addr % PAGE_SIZE == 0);
		
		while (start != end) {
			uint64_t &P1 = get_P1(start);
			P1 = src_addr | PTE_PRESENT | PTE_WRITABLE
				| PTE_CACHE_DISABLE | PTE_WRITE_THROUGH;
			
			start += PAGE_SIZE;
			src_addr += PAGE_SIZE;
		}
	}
	
	char * allocate_virtual_memory(uint64_t size) {
		size = Utils::round_up(size, HUGE_PAGE_SIZE);
		
		if (vaddr_break - kernel_break < size) {
			return NULL;
		} else {
			vaddr_break -= size;
			return (char *) vaddr_break;
		}
	}
	
	bool can_allocate_virtual_memory(uint64_t size) {
		size = Utils::round_up(size, HUGE_PAGE_SIZE);
		
		return vaddr_break - kernel_break >= size;
	}
	
	bool user_writable_check(uint64_t addr) {
		if (addr < kernel_break) {
			return false;
		} else if (addr >= vaddr_break) {
			return false;
		}
		
		uint64_t flags = PTE_USER | PTE_WRITABLE;
		if ((get_P1(addr) & flags) != flags) {
			return false;
		} else {
			return true;
		}
	}
}
