#include <inc/multiboot2_loader.h>
#include <inc/multiboot2.h>

#include <stdio.h>

namespace multiboot2_loader {
	static void load_mmap(struct multiboot_tag_mmap *mmap) {
		printf("mmap size = %u\n", mmap->size);
		printf("mmap entry_size = %u\n", mmap->entry_size);
		
		multiboot_memory_map_t *e = mmap->entries;
		while ((unsigned long) e != (unsigned long) mmap + mmap->size) {
			printf("base addr = %08llx (%.1lf MB), len = %08llx (%.1lf MB), type = %u\n",
				e->addr, e->addr / 1048576.0, e->len, e->len / 1048576.0, e->type);
			
			e = (multiboot_memory_map_t *) ((unsigned long) e + mmap->entry_size);
		}
	}
	
	void load(void *multiboot_addr) {
		struct multiboot_tag *tag = (struct multiboot_tag *) multiboot_addr + 1;
		
		while (tag->type != MULTIBOOT_TAG_TYPE_END) {
			switch (tag->type) {
				case MULTIBOOT_TAG_TYPE_MMAP:
					load_mmap((struct multiboot_tag_mmap *) tag);
					break;
			}
			
			tag += (tag->size + 7u) / 8u;
		}
	}
}
