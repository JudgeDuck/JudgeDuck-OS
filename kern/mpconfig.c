// Search for and parse the multiprocessor configuration table
// See http://developer.intel.com/design/pentium/datashts/24201606.pdf

#include <inc/types.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/x86.h>
#include <inc/mmu.h>
#include <inc/env.h>
#include <kern/cpu.h>
#include <kern/pmap.h>

struct CpuInfo cpus[NCPU];  // index: apic_id
int cpu_apic_id[NCPU];  // index: cpu_id
struct CpuInfo *bootcpu;
int ismp;
int ncpu;

// Per-CPU kernel stacks
unsigned char percpu_kstacks[NCPU][KSTKSIZE]
__attribute__ ((aligned(PGSIZE)));


// See MultiProcessor Specification Version 1.[14]

struct mp {             // floating pointer [MP 4.1]
	uint8_t signature[4];           // "_MP_"
	physaddr_t physaddr;            // phys addr of MP config table
	uint8_t length;                 // 1
	uint8_t specrev;                // [14]
	uint8_t checksum;               // all bytes must add up to 0
	uint8_t type;                   // MP system config type
	uint8_t imcrp;
	uint8_t reserved[3];
} __attribute__((__packed__));

struct mpconf {         // configuration table header [MP 4.2]
	uint8_t signature[4];           // "PCMP"
	uint16_t length;                // total table length
	uint8_t version;                // [14]
	uint8_t checksum;               // all bytes must add up to 0
	uint8_t product[20];            // product id
	physaddr_t oemtable;            // OEM table pointer
	uint16_t oemlength;             // OEM table length
	uint16_t entry;                 // entry count
	physaddr_t lapicaddr;           // address of local APIC
	uint16_t xlength;               // extended table length
	uint8_t xchecksum;              // extended table checksum
	uint8_t reserved;
	uint8_t entries[0];             // table entries
} __attribute__((__packed__));

struct mpproc {         // processor table entry [MP 4.3.1]
	uint8_t type;                   // entry type (0)
	uint8_t apicid;                 // local APIC id
	uint8_t version;                // local APIC version
	uint8_t flags;                  // CPU flags
	uint8_t signature[4];           // CPU signature
	uint32_t feature;               // feature flags from CPUID instruction
	uint8_t reserved[8];
} __attribute__((__packed__));

// mpproc flags
#define MPPROC_BOOT 0x02                // This mpproc is the bootstrap processor

// Table entry types
#define MPPROC    0x00  // One per processor
#define MPBUS     0x01  // One per bus
#define MPIOAPIC  0x02  // One per I/O APIC
#define MPIOINTR  0x03  // One per bus interrupt source
#define MPLINTR   0x04  // One per system interrupt source

// ACPI structs

struct ACPI_RDSP {
	uint8_t signature[8];
	uint8_t checksum;
	uint8_t oem_id[6];
	uint8_t revision;
	uint32_t rsdt_addr_phys;
	uint32_t length;
	uint64_t xsdt_addr_phys;
	uint8_t xchecksum;
	uint8_t reserved[3];
} __attribute__((__packed__));

struct ACPI_desc_header {
	uint8_t signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint8_t oem_tableid[8];
	uint32_t oem_revision;
	uint8_t creator_id[4];
	uint32_t creator_revision;
} __attribute__((__packed__));

struct ACPI_RSDT {
	struct ACPI_desc_header header;
	uint32_t entry[0];
} __attribute__((__packed__));

struct ACPI_MADT {
	struct ACPI_desc_header header;
	uint32_t lapic_addr_phys;
	uint32_t flags;
	uint8_t table[0];
} __attribute__((__packed__));

#define MADT_TYPE_LAPIC 0
#define MADT_TYPE_IOAPIC 1
#define MADT_TYPE_INT_SRC_OVERRIDE 2
#define MADT_TYPE_NMI_INT_SRC 3
#define MADT_TYPE_LAPIC_NMI 4

struct MADT_LAPIC {
	uint8_t type;
	uint8_t length;
	uint8_t acpi_id;
	uint8_t apic_id;
	uint32_t flags;
} __attribute__((__packed__));

#define APIC_LAPIC_ENABLED 1

// ================

static uint8_t
sum(void *addr, int len)
{
	int i, sum;

	sum = 0;
	for (i = 0; i < len; i++)
		sum += ((uint8_t *)addr)[i];
	return sum;
}

static struct ACPI_RDSP *scan_rdsp(uint32_t base, uint32_t len) {
	char *p;
	for (p = KADDR(base); len >= sizeof(struct ACPI_RDSP); len -= 4, p += 4) {
		if (memcmp(p, "RSD PTR ", 8) == 0 && sum(p, 20) == 0) {
			return (struct ACPI_RDSP *) p;
		}
	}
	return (struct ACPI_RDSP *) 0;  
}

static struct ACPI_RDSP *find_rdsp() {
	struct ACPI_RDSP *rdsp;
	uint32_t pa;
	pa = *((uint16_t *) KADDR(0x40e)) << 4;
	if (pa && (rdsp = scan_rdsp(pa, 1024))) {
		return rdsp;
	}
	return scan_rdsp(0xe0000, 0x20000);
}

// Look for an MP structure in the len bytes at physical address addr.
static struct mp *
mpsearch1(physaddr_t a, int len)
{
	struct mp *mp = KADDR(a), *end = KADDR(a + len);

	for (; mp < end; mp++)
		if (memcmp(mp->signature, "_MP_", 4) == 0 &&
		    sum(mp, sizeof(*mp)) == 0)
			return mp;
	return NULL;
}

// Search for the MP Floating Pointer Structure, which according to
// [MP 4] is in one of the following three locations:
// 1) in the first KB of the EBDA;
// 2) if there is no EBDA, in the last KB of system base memory;
// 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
static struct mp *
mpsearch(void)
{
	uint8_t *bda;
	uint32_t p;
	struct mp *mp;

	static_assert(sizeof(*mp) == 16);

	// The BIOS data area lives in 16-bit segment 0x40.
	bda = (uint8_t *) KADDR(0x40 << 4);

	// [MP 4] The 16-bit segment of the EBDA is in the two bytes
	// starting at byte 0x0E of the BDA.  0 if not present.
	if ((p = *(uint16_t *) (bda + 0x0E))) {
		p <<= 4;	// Translate from segment to PA
		if ((mp = mpsearch1(p, 1024)))
			return mp;
	} else {
		// The size of base memory, in KB is in the two bytes
		// starting at 0x13 of the BDA.
		p = *(uint16_t *) (bda + 0x13) * 1024;
		if ((mp = mpsearch1(p - 1024, 1024)))
			return mp;
	}
	return mpsearch1(0xF0000, 0x10000);
}

// Search for an MP configuration table.  For now, don't accept the
// default configurations (physaddr == 0).
// Check for the correct signature, checksum, and version.
static struct mpconf *
mpconfig(struct mp **pmp)
{
	struct mpconf *conf;
	struct mp *mp;

	if ((mp = mpsearch()) == 0)
		return NULL;
	if (mp->physaddr == 0 || mp->type != 0) {
		cprintf("SMP: Default configurations not implemented\n");
		return NULL;
	}
	conf = (struct mpconf *) KADDR(mp->physaddr);
	if (memcmp(conf, "PCMP", 4) != 0) {
		cprintf("SMP: Incorrect MP configuration table signature\n");
		return NULL;
	}
	if (sum(conf, conf->length) != 0) {
		cprintf("SMP: Bad MP configuration checksum\n");
		return NULL;
	}
	if (conf->version != 1 && conf->version != 4) {
		cprintf("SMP: Unsupported MP version %d\n", conf->version);
		return NULL;
	}
	if ((sum((uint8_t *)conf + conf->length, conf->xlength) + conf->xchecksum) & 0xff) {
		cprintf("SMP: Bad MP configuration extended checksum\n");
		return NULL;
	}
	*pmp = mp;
	return conf;
}

static void read_mem(void *dstva, void *srcpa, int len) {
	void *endpa = (char *) srcpa + len;
	boot_map_region(kern_pgdir, 0, ROUNDDOWN((uint32_t) endpa, PGSIZE) - ROUNDDOWN((uint32_t) srcpa, PGSIZE) + PGSIZE, ROUNDDOWN((uint32_t) srcpa, PGSIZE), PTE_W);
	lcr3(PADDR(kern_pgdir));
	uint32_t start = ROUNDDOWN((uint32_t) srcpa, PGSIZE);
	uint32_t end = ROUNDDOWN((uint32_t) endpa, PGSIZE);
	while (start != end) {
		invlpg((void *) start);
		start += PGSIZE;
	}
	invlpg((void *) start);
	memcpy(dstva, (void *)(((uint32_t) srcpa) % PGSIZE), len);
}

int find_cpu_index(int apic_id) {
	for (int i = 0; i < ncpu; i++) {
		if (cpus[i].cpu_id == apic_id) {
			return i;
		}
	}
	return 0;
}

void
mp_init(void)
{
	struct mp *mp;
	struct mpconf *conf;
	struct mpproc *proc;
	uint8_t *p;
	unsigned int i;

	bootcpu = &cpus[0];
	if ((conf = mpconfig(&mp)) == 0)
		return;
	ismp = 1;
	lapicaddr = conf->lapicaddr;
	
	struct ACPI_RDSP *rdsp = find_rdsp();
	cprintf("rdsp_v = %p\n", rdsp);
	
	struct ACPI_RSDT *rsdt_p = (void *)(rdsp->rsdt_addr_phys);
	cprintf("rsdt_p = %p\n", rsdt_p);
	
	static struct ACPI_RSDT rsdt;
	read_mem(&rsdt, rsdt_p, sizeof(struct ACPI_RSDT));
	
	struct ACPI_MADT *madt_p = 0;
	int count = (rsdt.header.length - sizeof(struct ACPI_RSDT)) / 4;
	for (int i = 0; i < count; i++) {
		char buf[4];
		read_mem(buf, (char *) rsdt_p + offsetof(struct ACPI_RSDT, entry) + i * 4, 4);
		struct ACPI_desc_header *hdr_p = (struct ACPI_desc_header *) *((uint32_t *) buf);
		read_mem(buf, (char *) hdr_p + offsetof(struct ACPI_desc_header, signature), 4);
		if (memcmp(buf, "APIC", 4) == 0) {
			madt_p = (struct ACPI_MADT *) hdr_p;
			break;
		}
	}
	cprintf("madt_p = %p\n", madt_p);
	
	static char madt_buf[23333];  // enough ???
	uint32_t madt_length;
	read_mem(&madt_length, (char *) madt_p + offsetof(struct ACPI_desc_header, length), 4);
	cprintf("madt_len = %d\n", madt_length);
	read_mem(madt_buf, madt_p, madt_length);
	struct ACPI_MADT *madt = (struct ACPI_MADT *) madt_buf;
	{
		void *p = madt->table;
		void *e = (char *) madt + madt_length;
		while (p < e) {
			int len = *((unsigned char *) p + 1);
			int type = *((unsigned char *) p);
			if (type == MADT_TYPE_LAPIC) {
				struct MADT_LAPIC *lapic = (struct MADT_LAPIC *) p;
				if (lapic->flags & APIC_LAPIC_ENABLED) {
					cprintf("ACPI: CPU #%d APIC id %d\n", ncpu, lapic->apic_id);
					if (ncpu < NCPU && lapic->apic_id < NCPU) {
						cpus[lapic->apic_id].cpu_id = ncpu;
						cpu_apic_id[ncpu] = lapic->apic_id;
						ncpu++;
					} else {
						cprintf("SMP: too many CPUs, CPU %d disabled\n", lapic->apic_id);
					}
				}
			}
			p += len;
		}
	}

	for (p = conf->entries, i = 0; i < conf->entry; i++) {
		break;
		switch (*p) {
		case MPPROC:
			proc = (struct mpproc *)p;
			if (proc->flags & MPPROC_BOOT)
				bootcpu = &cpus[proc->apicid];
			p += sizeof(struct mpproc);
			continue;
		case MPBUS:
		case MPIOAPIC:
		case MPIOINTR:
		case MPLINTR:
			p += 8;
			continue;
		default:
			cprintf("mpinit: unknown config type %x\n", *p);
			ismp = 0;
			i = conf->entry;
		}
	}

	bootcpu->cpu_status = CPU_STARTED;
	if (!ismp) {
		// Didn't like what we found; fall back to no MP.
		ncpu = 1;
		lapicaddr = 0;
		cprintf("SMP: configuration not found, SMP disabled\n");
		return;
	}
	cprintf("SMP: CPU %d found %d CPU(s)\n", bootcpu->cpu_id,  ncpu);

	if (mp->imcrp) {
		// [MP 3.2.6.1] If the hardware implements PIC mode,
		// switch to getting interrupts from the LAPIC.
		cprintf("SMP: Setting IMCR to switch from PIC mode to symmetric I/O mode\n");
		outb(0x22, 0x70);   // Select IMCR
		outb(0x23, inb(0x23) | 1);  // Mask external interrupts.
	}
}
