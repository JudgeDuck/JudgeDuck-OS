#include <stdio.h>
#include <math.h>
#include <iostream>

#include <inc/multiboot2_loader.hpp>
#include <inc/lapic.hpp>
#include <inc/memory.hpp>
#include <inc/pic.hpp>
#include <inc/trap.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>

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

	Time::set_tsc_per_sec(2208000000u); // TODO: set correct value
	
	Memory::init();
	
	Trap::init();
	Trap::enable();
	
	// TODO
	LINFO() << "Welcome to JudgeDuck-OS-64 !!!";

	LAPIC::timer_single_shot_ns((int) 1e9);
	while (1);
}
