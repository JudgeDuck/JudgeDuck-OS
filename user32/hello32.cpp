#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <x86intrin.h>

static void print_hello() {
	printf("Hello world from i386!  curr tsc = %llu\n", __rdtsc());
	
	int a, b;
	scanf("%d%d", &a, &b);
	printf("a + b = %d (from stdin), ", a + b);
	
	double e = 0, tmp = 1;
	for (int i = 1; i <= 50; i++) {
		e += tmp;
		tmp /= i;
	}
	printf("e = %.15lf, ", e);
	printf("pi = %.15lf", 2 * atan2(1, 0));
}

char A[3 << 20];
int B[] = {2, 3, 4, 1, 5};
const int C[] = {5, 4, 3, 2, 1};
char *p;

int main() {
	print_hello();
	
	fprintf(stderr, "stderr working");
	
	memset(A, 0, sizeof(A));
	
	const int SIZE = 50 << 20;
	p = new char[SIZE];
	memset(p, 0, SIZE);
	delete[] p;
	fprintf(stderr, ", memset %.1lf MiB ok", SIZE / 1048576.0);
	
	for (int i = 0; i < 1000000000; i++) {
		__asm__ volatile ("");
	}
	
	return 321;
}
