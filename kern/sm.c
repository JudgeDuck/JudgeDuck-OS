#include <kern/cpu.h>
#include <inc/stdio.h>
#include <inc/assert.h>
#include <kern/time.h>
#include <kern/pmap.h>
#include <inc/x86.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned uint;

struct SMBIOSEntryPoint {
	char EntryPointString[4];    //This is _SM_
	uchar Checksum;              //This value summed with all the values of the table, should be 0 (overflow)
	uchar Length;                //Length of the Entry Point Table. Since version 2.1 of SMBIOS, this is 0x1F
	uchar MajorVersion;          //Major Version of SMBIOS
	uchar MinorVersion;          //Minor Version of SMBIOS
	ushort MaxStructureSize;     //Maximum size of a SMBIOS Structure (we will se later)
	uchar EntryPointRevision;    //...
	char FormattedArea[5];       //...
	char EntryPointString2[5];   //This is _DMI_
	uchar Checksum2;             //Checksum for values from EntryPointString2 to the end of table
	ushort TableLength;          //Length of the Table containing all the structures
	uint TableAddress;	     //Address of the Table
	ushort NumberOfStructures;   //Number of structures in the table
	uchar BCDRevision;           //Unused
};

struct SMBIOSHeader {
	uchar Type;
	uchar Length;
	ushort Handle;
};

static struct SMBIOSEntryPoint *entry;
int external_clock_frequency;

static void
get_sm_entry()
{
	char *mem = (char *) 0x000F0000;
    int length, i;
    unsigned char checksum;
    while ((unsigned int) mem < 0x00100000) {
        if (mem[0] == '_' && mem[1] == 'S' && mem[2] == 'M' && mem[3] == '_') {
            length = mem[5];
            checksum = 0;
            for(i = 0; i < length; i++) {
                checksum += mem[i];
            }
            if(checksum == 0) break;
        }
        mem += 16;
    }
	cprintf("SM entry %p\n", mem);
	if ((unsigned int) mem == 0x00100000) {
        panic("No SMBIOS found!");
    }
	entry = (struct SMBIOSEntryPoint *) mem;
}

static size_t
sm_table_len(struct SMBIOSHeader *hd)
{
    size_t i;
    const char *strtab = (char *)hd + hd->Length;
    // Scan until we find a double zero byte
    for (i = 1; strtab[i - 1] != '\0' || strtab[i] != '\0'; i++)
        ;
    return hd->Length + i + 1;
}

static struct SMBIOSHeader *cur_head;
static struct SMBIOSHeader *sm_table_end;

void
sm_init_part1()
{
	cprintf("sm_init_part1\n");
	get_sm_entry();
	cur_head = (struct SMBIOSHeader *) (entry->TableAddress);
	sm_table_end = (struct SMBIOSHeader *) (entry->TableAddress + entry->TableLength);
}

static int get_tsc_clock_ratio(uint32_t *a, uint32_t *b) {
	uint32_t _a, _b, _c, _d;
	cpuid(0x15, &_a, &_b, &_c, &_d);
	if (_b == 0) {
		return -1;
	} else {
		*a = _b;
		*b = _a;
		return 0;
	}
}

static uint32_t get_base_freq() {
	uint32_t _a, _b, _c, _d;
	cpuid(0x16, &_a, &_b, &_c, &_d);
	return _a;
}

void
sm_init_part2()
{
	cprintf("sm_init_part2\n");
	while (cur_head < sm_table_end) {
		// assume two pages are enough ???
		boot_map_region(kern_pgdir, 0, PGSIZE, ROUNDDOWN((uint32_t) cur_head, PGSIZE), PTE_W);
		boot_map_region(kern_pgdir, PGSIZE, PGSIZE, ROUNDDOWN((uint32_t) cur_head, PGSIZE) + PGSIZE, PTE_W);
		lcr3(PADDR(kern_pgdir));
		invlpg(0);
		invlpg((void *) PGSIZE);
		
		struct SMBIOSHeader *head = (struct SMBIOSHeader *) (((uint32_t) cur_head) % PGSIZE);
		if (head->Type != 4) {
			cur_head = (struct SMBIOSHeader *) ((void *) cur_head + sm_table_len(head));
			continue;
		}
		unsigned short max_speed = *(unsigned short *) ((void *) head + 0x14);
		unsigned short cur_speed = *(unsigned short *) ((void *) head + 0x16);
		cprintf("max speed = %u\n", max_speed);
		cprintf("cur speed = %u\n", cur_speed);
		external_clock_frequency = *(unsigned short *) ((void *) head + 0x12);
		cprintf("detected ext clock freq = %d MHz\n", external_clock_frequency);
		
		uint32_t base_freq;
		uint32_t tsc_clock_ratio_a, tsc_clock_ratio_b;
		base_freq = get_base_freq();
		if (get_tsc_clock_ratio(&tsc_clock_ratio_a, &tsc_clock_ratio_b) < 0) {
			panic("gg get tsc clock ratio failed\n");
		}
		cprintf("base_freq = %u MHz, TSC/clock = %u/%u\n", base_freq, tsc_clock_ratio_a, tsc_clock_ratio_b);
		
		if (base_freq != 0) {
			external_clock_frequency = 1ull * base_freq * tsc_clock_ratio_b / tsc_clock_ratio_a;
			cur_speed = base_freq;
		}
		
		if (cur_speed != 0) {
			set_tsc_frequency(cur_speed * 1000000ull);
		}
		if (external_clock_frequency == 0) break;
		return;
	}
	external_clock_frequency = 1000;
	cprintf("cannot detected ext clock freq, assume 1000\n");
}
