#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <inc/smbios.hpp>
#include <inc/logger.hpp>
namespace SMBIOS {
	struct SMBIOSHeader {
		uint8_t type;
		uint8_t length;
		uint16_t handle;
	} __attribute__((packed));
	
	struct SMBIOSEntryPoint {
		char EntryPointString[4];    //This is _SM_
		uint8_t Checksum;              //This value summed with all the values of the table, should be 0 (overflow)
		uint8_t Length;                //Length of the Entry Point Table. Since version 2.1 of SMBIOS, this is 0x1F
		uint8_t MajorVersion;          //Major Version of SMBIOS
		uint8_t MinorVersion;          //Minor Version of SMBIOS
		uint16_t MaxStructureSize;     //Maximum size of a SMBIOS Structure (we will se later)
		uint8_t EntryPointRevision;    //...
		char FormattedArea[5];       //...
		char EntryPointString2[5];   //This is _DMI_
		uint8_t Checksum2;             //Checksum for values from EntryPointString2 to the end of table
		uint16_t TableLength;          //Length of the Table containing all the structures
		uint32_t TableAddress;	     //Address of the Table
		uint16_t NumberOfStructures;   //Number of structures in the table
		uint8_t BCDRevision;           //Unused
	} __attribute__((packed));
	
	uint32_t ext_clock_freq;
	
	static SMBIOSEntryPoint * find_sm_entry() {
		char *mem = (char *) 0x000F0000;
		unsigned char checksum;
		while ((uint64_t) mem < 0x00100000) {
			if (mem[0] == '_' && mem[1] == 'S' && mem[2] == 'M' && mem[3] == '_') {
				int length = mem[5];
				checksum = 0;
				for (int i = 0; i < length; i++) {
					checksum += mem[i];
				}
				if (checksum == 0) break;
			}
			mem += 16;
		}
		assert((uint64_t) mem != 0x00100000);
		return (SMBIOSEntryPoint *) mem;
	}
	
	static size_t get_length(SMBIOSHeader *header) {
		const char *strtab = (char *) header + header->length;
		
		// Scan until we find a double zero byte
		size_t i = 1;
		while (strtab[i - 1] != '\0' || strtab[i] != '\0') {
			i++;
		}
		
		return header->length + i + 1;
	}
	
	void init() {
		LINFO_ENTER();
		
		SMBIOSHeader *header = (SMBIOSHeader *) (uint64_t) find_sm_entry()->TableAddress;
		while (header->type != 127u) {
			if (header->type == 4) {
				ext_clock_freq = * (uint16_t *) ((char *) header + 0x12);
				printf("Detected ext clock freq = %d MHz\n", ext_clock_freq);
				// TODO: Convert 83 MHz to 83.33333 MHz ???
			}
			
			header = (SMBIOSHeader *) ((char *) header + get_length(header));
		}
		
		if (!ext_clock_freq) {
			ext_clock_freq = 1000;
			printf("Cannot detect ext clock freq, assuming 1000\n");
		}
	}
}
