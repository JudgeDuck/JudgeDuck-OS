#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <inc/multiboot2_loader.hpp>
#include <inc/multiboot2.h>
#include <inc/memory.hpp>
#include <inc/utils.hpp>
#include <inc/logger.hpp>

unsigned __multiboot_addr;
extern uint64_t ebss;

namespace Multiboot2_Loader {
	static void load_mmap(struct multiboot_tag_mmap *mmap) {
		multiboot_memory_map_t *e = mmap->entries;
		while ((unsigned long) e != (unsigned long) mmap + mmap->size) {
			LDEBUG("base %08llx (%.1lf MiB), len %08llx (%.1lf MiB), type %u",
				e->addr, e->addr / 1048576.0, e->len, e->len / 1048576.0, e->type);
			
			if (e->type == MULTIBOOT_MEMORY_AVAILABLE) {
				uint64_t start = Utils::round_up((uint64_t) e->addr, Memory::HUGE_PAGE_SIZE);
				uint64_t end = Utils::round_down((uint64_t) e->addr + (uint64_t) e->len, Memory::HUGE_PAGE_SIZE);
				if (start < (uint64_t) &ebss) {
					start = Utils::round_up((uint64_t) &ebss, Memory::HUGE_PAGE_SIZE);
				}
				for (uint64_t va = start; va < end; va += Memory::HUGE_PAGE_SIZE) {
					Memory::register_available_huge_page((void *) va);
				}
			}
			
			e = (multiboot_memory_map_t *) ((unsigned long) e + mmap->entry_size);
		}
	}
	
	static const int MAX_LENGTH = 256;
	
	static char _command_line[MAX_LENGTH];
	const char *command_line = _command_line;
	
	void load() {
		LDEBUG_ENTER_RET();
		
		void *multiboot_addr = (void *) (unsigned long) __multiboot_addr;
		
		struct multiboot_tag *tag = (struct multiboot_tag *) multiboot_addr + 1;
		
		while (tag->type != MULTIBOOT_TAG_TYPE_END) {
			switch (tag->type) {
				case MULTIBOOT_TAG_TYPE_MMAP:
					load_mmap((struct multiboot_tag_mmap *) tag);
					break;
				
				case MULTIBOOT_TAG_TYPE_CMDLINE:
					strncpy(_command_line, ((struct multiboot_tag_string *) tag)->string, MAX_LENGTH - 1);
					LDEBUG("cmdline: %s", command_line);
					break;
			}
			
			tag += (tag->size + 7u) / 8u;
		}
	}
}
