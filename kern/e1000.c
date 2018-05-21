#include <kern/e1000.h>
#include <kern/pmap.h>
#include <kern/env.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/x86.h>

// LAB 6: Your driver code here

volatile void *e1000 = NULL;
#define TQSIZE ((PGSIZE) / sizeof(struct TransDesc))
#define RQSIZE ((PGSIZE * 2) / sizeof(struct RecvDesc))
volatile struct TransDesc tq[TQSIZE] __attribute__((aligned(PGSIZE)));
volatile struct RecvDesc rq[RQSIZE] __attribute__((aligned(PGSIZE)));

static bool firstTrans = true;

static int
e1000_or_e1000e_attach(struct pci_func *pcif, unsigned maxMTA)
{
	assert(sizeof(struct TransDesc) == 16);
	assert(sizeof(struct RecvDesc) == 16);
	
	pci_func_enable(pcif);
	e1000 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
	cprintf("e1000 status = %x\n", *(uint32_t *) (e1000 + 0x8));
	// for(unsigned i = 1; i; i++) asm volatile("");
	
	uint32_t tdbal = PADDR((void *) tq);
	*(volatile uint32_t *) (e1000 + 0x3800) = tdbal;      // TDBAL
	*(volatile uint32_t *) (e1000 + 0x3804) = 0;          // TDBAH
	*(volatile uint32_t *) (e1000 + 0x3808) = sizeof(tq); // TDLEN
	*(volatile uint32_t *) (e1000 + 0x3810) = 0;          // TDH
	*(volatile uint32_t *) (e1000 + 0x3818) = 0;          // TDT
	uint32_t tctl = (1 << 1) | (1 << 3) | (0x10 << 4) | (0x40 << 12);
	*(volatile uint32_t *) (e1000 + 0x400) = tctl;        // TCTL
	// *(volatile uint32_t *) (e1000 + 0x410) = 0x60500a;    // TIPG
	*(volatile uint32_t *) (e1000 + 0x410) = 0xa | (0x8 << 10) | (0xc << 20);    // TIPG
	
	*(volatile uint32_t *) (e1000 + 0x5400) = 0x12005452; // RAL
	uint32_t rah = 0x5634 | (1u << 31);
	*(volatile uint32_t *) (e1000 + 0x5404) = rah;        // RAH
	for(volatile void *c = e1000 + 0x5200; c < e1000 + maxMTA; ++c)
		*(volatile char *) c = 0;                         // MTA
	*(volatile uint32_t *) (e1000 + 0xd0) = 0;            // IMS
	uint32_t rdbal = PADDR((void *) rq);
	*(volatile uint32_t *) (e1000 + 0x2800) = rdbal;      // RDBAL
	*(volatile uint32_t *) (e1000 + 0x2804) = 0;          // RDBAH
	*(volatile uint32_t *) (e1000 + 0x2808) = sizeof(rq); // RDLEN
	*(volatile uint32_t *) (e1000 + 0x2810) = 0;          // RDH
	*(volatile uint32_t *) (e1000 + 0x2818) = RQSIZE - 1; // RDT
	for(int i = 0; i < RQSIZE; i++)
	{
		struct PageInfo *pp = page_alloc(0);
		if(!pp)
			panic("fail to alloc memory for e1000 recv\n");
		rq[i].addr = page2pa(pp);
	}
	uint32_t rctl = (1 << 1) | (1 << 15) | (3 << 16) | (1 << 25) | (1 << 26);
	*(volatile uint32_t *) (e1000 + 0x100) = rctl;        // RCTL
	
	cprintf("e1000 status = %x\n", *(uint32_t *) (e1000 + 0x8));
	// for(unsigned i = 1; i; i++) asm volatile("");
	
	firstTrans = true;
	return 0;
}

int
e1000_attach(struct pci_func *pcif)
{
	return e1000_or_e1000e_attach(pcif, 0x5400);
}

int
e1000e_attach(struct pci_func *pcif)
{
	return e1000_or_e1000e_attach(pcif, 0x5280);
}

int
try_transmit(physaddr_t pa, int cnt)
{
	uint32_t tdt = *(volatile uint32_t *) (e1000 + 0x3818);
	if(tdt + 1 == TQSIZE)
		firstTrans = false;
	if(!firstTrans && !(tq[(tdt + 1) % TQSIZE].status & 1))
		return -E_NO_MEM;
	struct TransDesc td;
	memset(&td, 0, sizeof(td));
	td.addr = pa;
	td.length = cnt;
	td.cmd &= ~(1 << 5);
	td.cmd |= (1 << 3) | (1 << 0);
	tq[tdt] = td;
	tdt = (tdt + 1) % TQSIZE;
	*(volatile uint32_t *) (e1000 + 0x3818) = tdt;
	return 0;
}

int
try_receive(struct jif_pkt *jp)
{
	uint32_t rdt = *(volatile uint32_t *) (e1000 + 0x2818);
	volatile struct RecvDesc *rd = rq + (rdt + 1) % RQSIZE;
	if(!(rd->status & 1))
		return -E_NO_MEM;
	pte_t *pte;
	lcr3(PADDR(curenv->env_pgdir));
	jp->jp_len = rd->length;
	boot_map_region(curenv->env_pgdir, 0, PGSIZE, rd->addr, PTE_W);
	invlpg(0);
	memcpy(jp->jp_data, NULL, jp->jp_len);
	lcr3(PADDR(kern_pgdir));
	rdt = (rdt + 1) % TQSIZE;
	*(volatile uint32_t *) (e1000 + 0x2818) = rdt;
	return 0;
}

