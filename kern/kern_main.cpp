#include <stdio.h>
#include <math.h>
#include <iostream>

#include <inc/multiboot2_loader.hpp>
#include <inc/smbios.hpp>
#include <inc/lapic.hpp>
#include <inc/memory.hpp>
#include <inc/pic.hpp>
#include <inc/trap.hpp>

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
	
	Multiboot2_Loader::load();
	
	SMBIOS::init();
	PIC::init();
	LAPIC::init();
	
	Memory::init();
	
	Trap::init();
	Trap::enable();
	
	// TODO
	printf("Welcome to JudgeDuck-OS-64 !!!\n");
	LAPIC::timer_single_shot_ns((int) 1e9);
	while (1);
}
