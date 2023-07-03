#include <string.h>
#include <assert.h>

#include <inc/acpi.hpp>
#include <inc/x86_64.hpp>
#include <inc/logger.hpp>
#include <inc/utils.hpp>

namespace ACPI {
	static uint64_t rsdp_addr;
	
	struct ACPI_RSDP {
		char signature[8];
		uint8_t checksum;
		char oem_id[6];
		uint8_t revision;
		uint32_t rsdt_addr_phys;
		uint32_t length;
		uint64_t xsdt_addr_phys;
		uint8_t xchecksum;
		uint8_t reserved[3];
	} __attribute__((packed));
	
	struct ACPI_desc_header {
		char signature[4];
		uint32_t length;
		uint8_t revision;
		uint8_t checksum;
		char oem_id[6];
		char oem_tableid[8];
		uint32_t oem_revision;
		char creator_id[4];
		uint32_t creator_revision;
	} __attribute__((packed));
	
	struct ACPI_RSDT {
		struct ACPI_desc_header header;
		uint32_t entry[0];
	} __attribute__((packed));
	
	struct ACPI_MADT {
		struct ACPI_desc_header header;
		uint32_t lapic_addr_phys;
		uint32_t flags;
		uint8_t table[0];
	} __attribute__((packed));
	
	struct ACPI_FADT {
		struct   ACPI_desc_header header;
		uint32_t FirmwareCtrl;
		uint32_t DSDT;
		uint8_t  Reserved;
		uint8_t  PreferredPowerManagementProfile;
		uint16_t SCI_Interrupt;
		uint32_t SMI_CommandPort;
		uint8_t  ACPI_Enable;
		uint8_t  ACPI_Disable;
		uint8_t  S4BIOS_REQ;
		uint8_t  PSTATE_Control;
		uint32_t PM1aEventBlock;
		uint32_t PM1bEventBlock;
		uint32_t PM1aControlBlock;
		// more fields are omitted
	} __attribute__((packed));
	
	static void switch_to_acpi_mode(ACPI_FADT *fadt) {
		if (fadt->SMI_CommandPort == 0) {
			return;
		}
		
		if (fadt->ACPI_Enable == 0) {
			return;
		}
		
		x86_64::outb(fadt->SMI_CommandPort, fadt->ACPI_Enable);
		while ((x86_64::inw(fadt->PM1aControlBlock) & 1) == 0);
		
		LDEBUG("Switched to ACPI mode");
	}
	
	static ACPI_MADT *madt;
	
	void init() {
		LDEBUG_ENTER_RET();
		
		if (rsdp_addr == 0) {
			LERROR("rsdp_addr is 0, ACPI is not available");
			Utils::GG_reboot();
		}
		
		ACPI_RSDP *rsdp = (ACPI_RSDP *) rsdp_addr;
		ACPI_RSDT *rsdt = (ACPI_RSDT *) (uint64_t) (rsdp->rsdt_addr_phys);
		ACPI_FADT *fadt = 0;
		int count = (rsdt->header.length - sizeof(ACPI_RSDT)) / 4;
		for (int i = 0; i < count; i++) {
			ACPI_desc_header *hdr = (ACPI_desc_header *) (uint64_t) rsdt->entry[i];
			if (memcmp(hdr->signature, "APIC", 4) == 0) {
				madt = (ACPI_MADT *) hdr;
			} else if (memcmp(hdr->signature, "FACP", 4) == 0) {
				fadt = (ACPI_FADT *) hdr;
			}
		}
		assert(madt != NULL);
		
		if (fadt) {
			switch_to_acpi_mode(fadt);
		}
	}
	
	void set_rsdp_addr(uint64_t addr) {
		rsdp_addr = addr;
	}
	
	uint64_t get_lapic_addr() {
		return madt->lapic_addr_phys;
	}
}
