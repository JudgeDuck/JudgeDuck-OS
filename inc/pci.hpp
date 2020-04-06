#ifndef DUCK_PCI_H
#define DUCK_PCI_H

#include <stdint.h>

namespace PCI {
	// PCI subsystem interface
	enum { pci_res_bus, pci_res_mem, pci_res_io, pci_res_max };
	
	struct Bus;
	
	struct Func {
		Bus *bus;     // Primary bus for bridges
		
		uint32_t dev;
		uint32_t func;
		
		uint32_t dev_id;
		uint32_t dev_class;
		
		uint32_t reg_base[6];
		uint32_t reg_size[6];
		uint8_t irq_line;
	};
	
	struct Bus {
		Func *parent_bridge;
		uint32_t busno;
	};
	
	int init();
	void pci_func_enable(Func *f);
	
	// Map PCI device to virtual memory
	// Returns mapped length (in bytes)
	// -1 indicates failure
	uint64_t map_device(uint32_t key1, uint32_t key2, uint64_t addr, uint64_t maxlen);
}

#endif