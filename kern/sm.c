#include <kern/cpu.h>
#include <inc/stdio.h>
#include <inc/assert.h>
#include <kern/time.h>

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

void
sm_init()
{
	cprintf("sm_init\n");
	// external_clock_frequency = 1000;
	// cprintf("force 1000\n");
	// return;
	get_sm_entry();
	// cprintf("Major Version: %d\n", (int) entry->MajorVersion);
	// cprintf("Minor Version: %d\n", (int) entry->MinorVersion);
	struct SMBIOSHeader *cur_head = (struct SMBIOSHeader *) (entry->TableAddress);
	int n = entry->NumberOfStructures;
	// cprintf("asdf %d\n", n);
	// for(char *c = (char *) entry->TableAddress; c < (char *) (entry->TableAddress + entry->TableLength); ++c) cprintf("%p -> %02x\n", c, (int) (unsigned char) *c);
	while(cur_head < (struct SMBIOSHeader *) (entry->TableAddress + entry->TableLength))
	{
		// cprintf("head %p, type %d\n", cur_head, (int) cur_head->Type);
		if(cur_head->Type != 4)
		{
			cur_head = (struct SMBIOSHeader *) ((void *) cur_head + sm_table_len(cur_head));
			continue;
		}
		unsigned short max_speed = *(unsigned short *) ((void *) cur_head + 0x14);
		unsigned short cur_speed = *(unsigned short *) ((void *) cur_head + 0x16);
		cprintf("max speed = %u\n", max_speed);
		cprintf("cur speed = %u\n", cur_speed);
		external_clock_frequency = *(unsigned short *) ((void *) cur_head + 0x12);
		cprintf("detected ext clock freq = %d MHz\n", external_clock_frequency);
		if (cur_speed != 0) {
			set_tsc_frequency(cur_speed * 1000000ull);
		}
		if(external_clock_frequency == 0) break;
		// for(int i = 0; i < 2000000000; i++) asm volatile("");
		return;
	}
	external_clock_frequency = 1000;
	cprintf("cannot detected ext clock freq, assume 1000\n");
	// for(int i = 0; i < 2000000000; i++) asm volatile("");
}
