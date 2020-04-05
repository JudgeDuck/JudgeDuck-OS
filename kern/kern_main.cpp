#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <iostream>

#include <inc/multiboot2_loader.hpp>
#include <inc/lapic.hpp>
#include <inc/memory.hpp>
#include <inc/pic.hpp>
#include <inc/trap.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>
#include <inc/elf.hpp>
#include <inc/x86_64.hpp>

static void print_hello() {
	printf("Hello world!\n");
	
	double e = 0, tmp = 1;
	for (int i = 1; i <= 20; i++) {
		e += tmp;
		tmp /= i;
	}
	printf("e = %.15lf\n", e);
	printf("pi = 2 * atan2(1, 0) = %.15lf\n", 2 * atan2(1, 0));
	
	std::cout << "std::cout works!" << std::endl;
}

int main() {
	print_hello();
	
	Logger::init();
	// TODO: disable debug logging for production
	
	Multiboot2_Loader::load();
	
	PIC::init();
	Timer::init();
	LAPIC::init();
	
	Memory::init();
	
	Trap::init();
	
	// TODO
	LINFO("Welcome to JudgeDuck-OS-64 !!!");
	
	extern const char _binary_hello_elf_start[];
	extern const char _binary_hello_elf_end[];
	int len = _binary_hello_elf_end - _binary_hello_elf_start;
	LDEBUG("start = %p, len = %d", _binary_hello_elf_start, len);
	
	Logger::set_log_level(Logger::LL_INFO);
	ELF::App64 app;
	const uint64_t memory_hard_limit = 100 << 20;
	assert(ELF::load(_binary_hello_elf_start, len, NULL, memory_hard_limit, app));
	auto res = ELF::run(app, 5e9);
	LINFO("time %.6lf ms, memory %d KiB (%.1lf MiB)",
		res.time_ns / 1e6, res.memory_kb, res.memory_kb / 1024.0);
	LINFO("tsc %lu, trap %u, retcode %d",
		res.time_tsc, res.trap_num, res.return_code);
	
	while (1) x86_64::hlt();
}
