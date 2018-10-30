#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>

int e1000_attach(struct pci_func *pcif);
int e1000e_attach(struct pci_func *pcif);

struct TransDesc
{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
} __attribute__((packed));

struct RecvDesc
{
	uint64_t addr;
	uint16_t length;
	uint16_t chksum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
} __attribute__((packed));

#endif	// JOS_KERN_E1000_H
