#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <algorithm>

#include <inc/multiboot2_loader.hpp>
#include <inc/lapic.hpp>
#include <inc/memory.hpp>
#include <inc/pic.hpp>
#include <inc/trap.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>
#include <inc/pci.hpp>
#include <inc/network_driver.hpp>
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
	
	PCI::init();
	NetworkDriver::init();
	
	LINFO("Welcome to JudgeDuck-OS-64 !!!");
	LINFO("ABI Version 0.02b");
	
	extern const char _binary_hello_elf_start[];
	extern const char _binary_hello_elf_end[];
	int len = _binary_hello_elf_end - _binary_hello_elf_start;
	LDEBUG("start = %p, len = %d", _binary_hello_elf_start, len);
	
	Logger::set_log_level(Logger::LL_INFO);
	ELF::App64 app;
	auto config = (ELF::App64Config) {
		.memory_hard_limit = 100 << 20,
		.stdin_ptr = "100 987\n",
		.stdin_size = 8,
		.stdout_max_size = 10240,
		.stderr_max_size = 10240,
	};
	assert(ELF::load(_binary_hello_elf_start, len, config, app));
	auto res = ELF::run(app, 5e9);
	LINFO("time %.6lf ms, memory %d KiB (%.1lf MiB)",
		res.time_ns / 1e6, res.memory_kb, res.memory_kb / 1024.0);
	LINFO("tsc %lu, trap %u, retcode %d",
		res.time_tsc, res.trap_num, res.return_code);
	LINFO("stdout size %lu, stderr size %lu",
		res.stdout_size, res.stderr_size);
	LINFO(">>> stdout content (first 256 bytes) <<<");
	fwrite(res.stdout_ptr, 1, std::min(256ul, res.stdout_size), stdout);
	putchar('\n');
	LINFO(">>> stderr content (first 256 bytes) <<<");
	fwrite(res.stderr_ptr, 1, std::min(256ul, res.stderr_size), stderr);
	putchar('\n');
	LINFO("========================================");
	
	while (1) x86_64::hlt();
}
