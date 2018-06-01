#include <inc/x86.h>
#include <inc/mmu.h>
#include <inc/stdio.h>

#include <kern/memdetect.h>

#define	IO_RTC		0x070
#define	MC_NVRAM_START	0xe
#define	MC_NVRAM_SIZE	50

#define NVRAM_BASELO	(MC_NVRAM_START + 7)
#define NVRAM_EXTLO	(MC_NVRAM_START + 9)
#define NVRAM_EXT16LO	(MC_NVRAM_START + 38)

static unsigned
mc146818_read(unsigned reg)
{
	outb(IO_RTC, reg);
	return inb(IO_RTC + 1);
}

static int
nvram_read(int r)
{
	return mc146818_read(r) | (mc146818_read(r + 1) << 8);
}

size_t
i386_detect_memory(void)
{
	size_t basemem, extmem, ext16mem, totalmem;
	size_t npages, npages_basemem;

	// Use CMOS calls to measure available base & extended memory.
	// (CMOS calls return results in kilobytes.)
	basemem = nvram_read(NVRAM_BASELO);
	extmem = nvram_read(NVRAM_EXTLO);
	ext16mem = nvram_read(NVRAM_EXT16LO) * 64;

	// Calculate the number of physical pages available in both base
	// and extended memory.
	if (ext16mem)
		totalmem = 16 * 1024 + ext16mem;
	else if (extmem)
		totalmem = 1 * 1024 + extmem;
	else
		totalmem = basemem;

	npages = totalmem / (PGSIZE / 1024);
	npages_basemem = basemem / (PGSIZE / 1024);

	cprintf("Physical memory: %uK available, base = %uK, extended = %uK\n",
		totalmem, basemem, totalmem - basemem);
	
	cprintf("Total pages: %u\n", npages);
	return npages;
}
