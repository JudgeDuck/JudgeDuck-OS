#include <inc/tls.h>
#include <stdio.h>
#include <math.h>

extern "C" void __duck64__libc_init();

static void init_libc() {
	extern void *__duck64__tls_base;
	TLS::set_fs(&__duck64__tls_base);
	__duck64__tls_base = &__duck64__tls_base;
	
	__duck64__libc_init();
}

static void print_hello() {
	printf("Hello world!\n");
	
	double e = 0, tmp = 1;
	for (int i = 1; i <= 20; i++) {
		e += tmp;
		tmp /= i;
	}
	printf("e = %.15lf\n", e);
	printf("pi = 2 * atan2(1, 0) = %.15lf\n", 2 * atan2(1, 0));
}

extern "C"
void kern_main() {
	init_libc();
	
	print_hello();
	
	while (1);
}
