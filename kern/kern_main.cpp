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
	LINFO("start = %p, len = %d", _binary_hello_elf_start, len);
	
	ELF::App64 app;
	assert(ELF::load(_binary_hello_elf_start, len, NULL, app));
	// TODO: time limit
	// TODO: return value
	ELF::run(app);
	
	while (1);
}
