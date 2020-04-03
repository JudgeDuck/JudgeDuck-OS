#include <elf.h>
#include <string.h>
#include <algorithm>

#include <inc/elf.hpp>
#include <inc/logger.hpp>
#include <inc/utils.hpp>
#include <inc/memory.hpp>
#include <inc/trap.hpp>
#include <inc/lapic.hpp>

namespace ELF {
	bool load(const char *buf, uint32_t len, void *dst, App64 &app) {
		LDEBUG_ENTER_RET();
		
		// Read elf header
		const Elf64_Ehdr *hdr = (const Elf64_Ehdr *) buf;
		if (len < sizeof(*hdr)) return false;
		if (hdr->e_ident[EI_MAG0] != ELFMAG0) return false;
		if (hdr->e_ident[EI_MAG1] != ELFMAG1) return false;
		if (hdr->e_ident[EI_MAG2] != ELFMAG2) return false;
		if (hdr->e_ident[EI_MAG3] != ELFMAG3) return false;
		if (hdr->e_ident[EI_CLASS] != ELFCLASS64) return false;
		if (hdr->e_ident[EI_DATA] != ELFDATA2LSB) return false;
		if (hdr->e_ident[EI_VERSION] != EV_CURRENT) return false;
		if (hdr->e_ident[EI_OSABI] != ELFOSABI_SYSV) return false;
		if (hdr->e_type != ET_EXEC) return false;
		if (hdr->e_machine != EM_X86_64) return false;
		if (hdr->e_version != EV_CURRENT) return false;
		uint64_t e_entry = (uint64_t) hdr->e_entry;
		uint64_t e_phoff = (uint64_t) hdr->e_phoff;
		uint64_t e_phentsize = (uint64_t) hdr->e_phentsize;
		uint64_t e_phnum = (uint64_t) hdr->e_phnum;
		LDEBUG() << "ELF Entry = 0x" << std::hex << e_entry << std::dec;
		if (e_phoff >= len) return false;
		if (e_phentsize >= len) return false;
		if (e_phnum >= len) return false;
		
		if (dst != NULL) {
			// TODO load to specific address
			unimplemented();
		}
		
		uint64_t min_vaddr = -1;
		uint64_t max_vaddr = 0;
		
		for (uint64_t i = 0; i < e_phnum; i++) {
			uint64_t off = e_phoff + i * e_phentsize;
			if (off + sizeof(Elf64_Phdr) > len) return false;
			const Elf64_Phdr *phdr = (const Elf64_Phdr *) ((uint64_t) buf + off);
			if (phdr->p_type != PT_LOAD) continue;
			uint64_t p_offset = (uint64_t) phdr->p_offset;
			uint64_t p_vaddr = (uint64_t) phdr->p_vaddr;
			uint64_t p_filesz = (uint64_t) phdr->p_filesz;
			uint64_t p_memsz = (uint64_t) phdr->p_memsz;
			uint64_t p_align = (uint64_t) phdr->p_align;
			
			if (p_offset > len || p_offset + p_filesz > len) return false;
			if (p_align != Memory::HUGE_PAGE_SIZE) return false;
			// TODO: more checks
			
			min_vaddr = std::min(min_vaddr, p_vaddr);
			max_vaddr = std::max(max_vaddr, p_vaddr + p_memsz);
			
			char *addr = (char *) p_vaddr;
			const char *src = buf + p_offset;
			memset(addr, 0, p_memsz);
			memcpy(addr, src, p_filesz);
			// TODO: clear garbage before loading
		}
		
		min_vaddr = Utils::round_down(min_vaddr, Memory::HUGE_PAGE_SIZE);
		max_vaddr = Utils::round_up(max_vaddr, Memory::HUGE_PAGE_SIZE);
		// TODO: check addresses
		
		// TODO: configurable memory limit
		const uint64_t memory_limit = 100 << 20;
		uint64_t stack_addr = min_vaddr + memory_limit;
		// One more page for arguments
		memset((void *) max_vaddr, 0, stack_addr - max_vaddr + Memory::HUGE_PAGE_SIZE);
		// TODO: set up page table
		
		LDEBUG() << "Load ELF ok!";
		
		app.entry = e_entry;
		app.rsp = stack_addr;
		return true;
	}
	
	RunResult run(const App64 &app, uint64_t time_limit_ns) {
		LDEBUG_ENTER_RET();
		
		RunResult res;
		Trap::run_user_64(app.entry, app.rsp, time_limit_ns,
			res.time_tsc, res.trap_num, res.return_code);
		res.time_ns = Timer::tsc_to_ns(res.time_tsc);
		
		return res;
	}
}
