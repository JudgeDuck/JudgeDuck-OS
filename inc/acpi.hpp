#ifndef DUCK_ACPI_H
#define DUCK_ACPI_H

#include <stdint.h>

namespace ACPI {
	void init();
	
	// Should be called before init()
	void set_rsdp_addr(uint64_t addr);
	
	// Should be called after init()
	uint64_t get_lapic_addr();
}

#endif
