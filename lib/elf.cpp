#include <elf.h>
#include <string.h>
#include <algorithm>

#include <inc/elf.hpp>
#include <inc/logger.hpp>
#include <inc/utils.hpp>
#include <inc/memory.hpp>
#include <inc/trap.hpp>
#include <inc/lapic.hpp>
#include <inc/x86_64.hpp>

using Memory::HUGE_PAGE_SIZE;
using Memory::PAGE_SIZE;

// [JudgeDuck-ABI, "Running"]
#define AT_DUCK 0x6b637564  // "duck"

using Utils::round_up;
using Utils::round_down;

namespace ELF {
	static inline void pushq(uint64_t &rsp, uint64_t val) {
		rsp -= 8;
		* (uint64_t *) rsp = val;
	}
	
	template <class T>
	static inline void push(uint64_t &rsp, const T &a) {
		rsp -= sizeof(a);
		* (T *) rsp = a;
	}
	
	bool load(const char *buf, uint32_t len,
		const App64Config &config, App64 &app) {
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
		if (hdr->e_ident[EI_OSABI] != ELFOSABI_SYSV &&
			hdr->e_ident[EI_OSABI] != ELFOSABI_LINUX) return false;
		if (hdr->e_type != ET_EXEC) return false;
		if (hdr->e_machine != EM_X86_64) return false;
		if (hdr->e_version != EV_CURRENT) return false;
		const uint64_t e_entry = (uint64_t) hdr->e_entry;
		const uint64_t e_phoff = (uint64_t) hdr->e_phoff;
		const uint64_t e_phentsize = (uint64_t) hdr->e_phentsize;
		const uint64_t e_phnum = (uint64_t) hdr->e_phnum;
		LDEBUG() << "ELF Entry = 0x" << std::hex << e_entry << std::dec;
		if (e_phoff >= len) return false;
		if (e_phentsize >= len) return false;
		if (e_phnum >= len) return false;
		
		uint64_t min_vaddr = -1;
		uint64_t max_vaddr = 0;
		
		for (uint64_t i = 0; i < e_phnum; i++) {
			const uint64_t off = e_phoff + i * e_phentsize;
			if (off + sizeof(Elf64_Phdr) > len) return false;
			const Elf64_Phdr *phdr = (const Elf64_Phdr *) ((uint64_t) buf + off);
			if (phdr->p_type != PT_LOAD) continue;
			const uint64_t p_offset = (uint64_t) phdr->p_offset;
			const uint64_t p_vaddr = (uint64_t) phdr->p_vaddr;
			const uint64_t p_filesz = (uint64_t) phdr->p_filesz;
			const uint64_t p_memsz = (uint64_t) phdr->p_memsz;
			const uint64_t p_align = (uint64_t) phdr->p_align;
			
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
		min_vaddr = round_down(min_vaddr, PAGE_SIZE);
		max_vaddr = round_up(max_vaddr, PAGE_SIZE);
		if (min_vaddr < Memory::get_kernel_break()) return false;
		if (max_vaddr <= min_vaddr || max_vaddr > Memory::get_vaddr_break()) return false;
		
		const uint64_t memory_limit = round_up(config.memory_hard_limit, PAGE_SIZE);
		if (max_vaddr - min_vaddr > memory_limit) return false;
		
		// [JudgeDuck-ABI, "Loading into Memory"]
		const uint64_t load_vaddr_start = 0x400000;
		// TODO: Support PIC programs with vaddr other than default value
		if (min_vaddr != load_vaddr_start) {
			LWARN("Programs with load address other than 0x400000 are not supported yet");
			return false;
		}
		const uint64_t load_vaddr_break = load_vaddr_start + memory_limit;
		if (load_vaddr_break < load_vaddr_start) return false;
		if (load_vaddr_break > Memory::get_vaddr_break()) return false;
		
		// Special region for stdin, stdout, stderr
		const uint64_t special_region_vaddr = load_vaddr_break;
		uint64_t special_region_break_curr = special_region_vaddr;
		
		// Allocate stdin
		const uint64_t stdin_alloc_size = round_up(config.stdin_size, PAGE_SIZE);
		if (stdin_alloc_size < config.stdin_size) return false;  // overflow?
		const uint64_t stdin_load_vaddr = special_region_break_curr;
		const uint64_t stdin_load_break = stdin_load_vaddr + stdin_alloc_size;
		if (stdin_load_break < stdin_load_vaddr) return false;  // overflow?
		special_region_break_curr = stdin_load_break;
		
		// Allocate stdout
		const uint64_t stdout_alloc_size = round_up(config.stdout_max_size, PAGE_SIZE);
		if (stdout_alloc_size < config.stdout_max_size) return false;  // overflow?
		const uint64_t stdout_load_vaddr = special_region_break_curr;
		const uint64_t stdout_load_break = stdout_load_vaddr + stdout_alloc_size;
		if (stdout_load_break < stdout_load_vaddr) return false;  // overflow?
		special_region_break_curr = stdout_load_break;
		
		// Allocate stderr
		const uint64_t stderr_alloc_size = round_up(config.stderr_max_size, PAGE_SIZE);
		if (stderr_alloc_size < config.stderr_max_size) return false;  // overflow?
		const uint64_t stderr_load_vaddr = special_region_break_curr;
		const uint64_t stderr_load_break = stderr_load_vaddr + stderr_alloc_size;
		if (stderr_load_break < stderr_load_vaddr) return false;  // overflow?
		special_region_break_curr = stderr_load_break;
		
		// End of special region
		const uint64_t special_region_break = special_region_break_curr;
		if (special_region_break < special_region_vaddr) return false;
		if (special_region_break > Memory::get_vaddr_break()) return false;
		
		// Clear memory and prepare to load
		memset((void *) load_vaddr_start, 0, special_region_break - load_vaddr_start);
		
		// Set up page flags
		Memory::set_page_flags_kernel(Memory::get_kernel_break(), load_vaddr_start);
		Memory::set_page_flags_user_writable(load_vaddr_start, special_region_break);
		Memory::set_page_flags_kernel(special_region_break, Memory::get_vaddr_break());
		
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
		
		// Ensure stdin region writable by reloading cr3
		x86_64::lcr3(x86_64::rcr3());
		
		// Load stdin
		memcpy((void *) stdin_load_vaddr, config.stdin_ptr, config.stdin_size);
		Memory::set_page_flags_user_readonly(stdin_load_vaddr, stdin_load_break);
		
		static DuckInfo_t duckinfo;
		duckinfo = (DuckInfo_t) {
			.abi_version = 21,  // 0.02a
			.stdin_ptr = (const char *) stdin_load_vaddr,
			.stdin_size = config.stdin_size,
			.stdout_ptr = (char *) stdout_load_vaddr,
			.stdout_size = config.stdout_max_size,
			.stderr_ptr = (char *) stderr_load_vaddr,
			.stderr_size = config.stderr_max_size,
		};
		
		// Set up environment variables [JudgeDuck-ABI, "Running"]
		uint64_t rsp = load_vaddr_break;
		push(rsp, duckinfo);
		const uint64_t duckinfo_ptr = rsp;
		const uint64_t rsp_auxv_end = rsp;
		pushq(rsp, 0);  // The end of the auxiliary vector
		pushq(rsp, duckinfo_ptr);  // duckinfo pointer
		pushq(rsp, AT_DUCK);  // The duck entry
		pushq(rsp, 0);  // The end of the environment strings
		pushq(rsp, 0);  // The end of the argument strings
		pushq(rsp, 0);  // The argument count
		
		// AMD64 ABI said rsp should be 16-bytes aligned
		if (rsp % 16 != 0) {
			uint64_t new_rsp = round_down(rsp, 16);
			memmove((void *) new_rsp, (void *) rsp, rsp_auxv_end - rsp);
			rsp = new_rsp;
		}
		
		// Check execution flags
		for (uint64_t i = 0; i < e_phnum; i++) {
			uint64_t off = e_phoff + i * e_phentsize;
			const Elf64_Phdr *phdr = (const Elf64_Phdr *) ((uint64_t) buf + off);
			if (phdr->p_type != PT_LOAD) continue;
			uint64_t p_vaddr = (uint64_t) phdr->p_vaddr;
			uint64_t p_memsz = (uint64_t) phdr->p_memsz;
			
			if (phdr->p_flags & PF_X) {
				Memory::set_page_flags_user_executable(
					round_down(p_vaddr, PAGE_SIZE),
					round_up(p_vaddr + p_memsz, PAGE_SIZE));
			}
		}
		
		// Clear access and dirty flags for measuring
		Memory::clear_access_and_dirty_flags(load_vaddr_start, load_vaddr_break);
		
		LDEBUG() << "Load ELF ok!";
		
		app.entry = e_entry;
		app.rsp = rsp;
		app.start_addr = load_vaddr_start;
		app.break_addr = load_vaddr_break;
		app.special_region_start_addr = special_region_vaddr;
		app.special_region_break_addr = special_region_break;
		app.duckinfo_orig = duckinfo;
		app.duckinfo_ptr = (DuckInfo_t *) duckinfo_ptr;
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
		
		// stdout
		res.stdout_ptr = app.duckinfo_orig.stdout_ptr;
		res.stdout_size = std::min(app.duckinfo_orig.stdout_size,
			app.duckinfo_ptr->stdout_size);
		
		// stderr
		res.stderr_ptr = app.duckinfo_orig.stderr_ptr;
		res.stderr_size = std::min(app.duckinfo_orig.stderr_size,
			app.duckinfo_ptr->stderr_size);
		
		return res;
	}
}
