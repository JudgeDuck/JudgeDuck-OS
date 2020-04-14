#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <x86intrin.h>

static void print_hello() {
	printf("Hello world!  curr tsc = %llu\n", __rdtsc());
	
	double e = 0, tmp = 1;
	for (int i = 1; i <= 20; i++) {
		e += tmp;
		tmp /= i;
	}
	printf("e = %.15lf, ", e);
	printf("pi = 2 * atan2(1, 0) = %.15lf\n", 2 * atan2(1, 0));
}

char A[3 << 20];
int B[] = {2, 3, 4, 1, 5};
const int C[] = {5, 4, 3, 2, 1};

int main() {
	print_hello();
	
	memset(A, 0, sizeof(A));
	
	int a, b;
	scanf("%d%d", &a, &b);
	printf("a + b = %d (read from stdin)\n", a + b);
	
	const int SIZE = 50 << 20;
	char *p = new char[SIZE];
	memset(p, 0, SIZE);
	delete[] p;
	printf("memset %.1lf MiB ok\n", SIZE / 1048576.0);
	
	for (int i = 0; i < 1000000000; i++) {
		__asm__ volatile ("");
	}
	
	fprintf(stderr, "stderr working\n");
	
	return 123;
}
