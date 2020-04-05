#include <elf.h>
#include <string.h>
#include <algorithm>

#include <inc/elf.hpp>
#include <inc/logger.hpp>
#include <inc/utils.hpp>
#include <inc/memory.hpp>
#include <inc/trap.hpp>
#include <inc/lapic.hpp>

using Memory::HUGE_PAGE_SIZE;
using Memory::PAGE_SIZE;

namespace ELF {
	static inline void pushq(uint64_t &rsp, uint64_t val) {
		rsp -= 8;
		* (uint64_t *) rsp = val;
	}
	
	bool load(const char *buf, uint32_t len, void *dst,
		uint64_t memory_hard_limit, App64 &app) {
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
			
			// Check in-file offset and size
			if (p_offset > len || p_offset + p_filesz > len) return false;
			
			// Check virtual address alignment [JudgeDuck-ABI, "Compilation and Linkage"]
			if (p_align != HUGE_PAGE_SIZE) return false;
			
			// Check virtual address and size
			if (p_memsz < p_filesz) return false;
			if (p_vaddr + p_memsz < p_vaddr) return false;  // overflow?
			
			// Update min and max
			min_vaddr = std::min(min_vaddr, p_vaddr);
			max_vaddr = std::max(max_vaddr, p_vaddr + p_memsz);
		}
		
		// TODO: Support huge page
		min_vaddr = Utils::round_down(min_vaddr, PAGE_SIZE);
		max_vaddr = Utils::round_up(max_vaddr, PAGE_SIZE);
		if (min_vaddr < Memory::get_kernel_break()) return false;
		if (max_vaddr <= min_vaddr || max_vaddr > Memory::get_vaddr_break()) return false;
		
		uint64_t memory_limit = memory_hard_limit;
		memory_limit = Utils::round_up(memory_limit, PAGE_SIZE);
		if (max_vaddr - min_vaddr > memory_limit) return false;
		
		uint64_t load_vaddr_start = 0x400000;  // [JudgeDuck-ABI, "Loading into Memory"]
		// TODO: Support PIC programs with vaddr other than default value
		if (min_vaddr != load_vaddr_start) {
			LWARN("Programs with load address other than 0x400000 are not supported yet");
			return false;
		}
		uint64_t load_vaddr_break = load_vaddr_start + memory_limit;
		if (load_vaddr_break < load_vaddr_start) return false;
		if (load_vaddr_break > Memory::get_vaddr_break()) return false;
		// TODO: Support file I/O
		
		// Clear memory and prepare to load
		memset((void *) load_vaddr_start, 0, load_vaddr_break - load_vaddr_start);
		
		// Set up page flags
		Memory::set_page_flags_kernel(Memory::get_kernel_break(), load_vaddr_start);
		Memory::set_page_flags_user_writable(load_vaddr_start, load_vaddr_break);
		Memory::set_page_flags_kernel(load_vaddr_break, Memory::get_vaddr_break());
		
		// Do actual loading
		for (uint64_t i = 0; i < e_phnum; i++) {
			uint64_t off = e_phoff + i * e_phentsize;
			const Elf64_Phdr *phdr = (const Elf64_Phdr *) ((uint64_t) buf + off);
			if (phdr->p_type != PT_LOAD) continue;
			uint64_t p_offset = (uint64_t) phdr->p_offset;
			uint64_t p_vaddr = (uint64_t) phdr->p_vaddr;
			uint64_t p_filesz = (uint64_t) phdr->p_filesz;
			uint64_t p_memsz = (uint64_t) phdr->p_memsz;
			
			// Copy from ELF file
			char *addr = (char *) p_vaddr;
			const char *src = buf + p_offset;
			memset(addr, 0, p_memsz);
			memcpy(addr, src, p_filesz);
		}
		
		// Set up environment variables [JudgeDuck-ABI, "Running"]
		uint64_t rsp = load_vaddr_break;
		pushq(rsp, 0);  // The end of the auxiliary vector
		pushq(rsp, 0);  // The end of the environment strings
		pushq(rsp, 0);  // The end of the argument strings
		pushq(rsp, 0);  // The argument count
		
		// Check execution flags
		for (uint64_t i = 0; i < e_phnum; i++) {
			uint64_t off = e_phoff + i * e_phentsize;
			const Elf64_Phdr *phdr = (const Elf64_Phdr *) ((uint64_t) buf + off);
			if (phdr->p_type != PT_LOAD) continue;
			uint64_t p_vaddr = (uint64_t) phdr->p_vaddr;
			uint64_t p_memsz = (uint64_t) phdr->p_memsz;
			
			if (phdr->p_flags & PF_X) {
				Memory::set_page_flags_user_executable(
					Utils::round_down(p_vaddr, PAGE_SIZE),
					Utils::round_up(p_vaddr + p_memsz, PAGE_SIZE));
			}
		}
		
		// Clear access and dirty flags for measuring
		Memory::clear_access_and_dirty_flags(load_vaddr_start, load_vaddr_break);
		
		LDEBUG() << "Load ELF ok!";
		
		app.entry = e_entry;
		app.rsp = rsp;
		app.start_addr = load_vaddr_start;
		app.break_addr = load_vaddr_break;
		return true;
	}
	
	RunResult run(const App64 &app, uint64_t time_limit_ns) {
		LDEBUG_ENTER_RET();
		
		RunResult res;
		Trap::run_user_64(app.entry, app.rsp, time_limit_ns,
			res.time_tsc, res.trap_num, res.return_code);
		res.time_ns = Timer::tsc_to_ns(res.time_tsc);
		res.memory_bytes = PAGE_SIZE *
			Memory::count_dirty_pages(app.start_addr, app.break_addr);
		res.memory_kb = res.memory_bytes / 1024;
		
		return res;
	}
}
