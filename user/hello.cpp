#include <stdio.h>

char a[3 << 20];
int b[] = {1, 2, 3, 4, 5};
const int c[] = {5, 4, 3, 2, 1};

int main() {
	for (int i = 0; i < 100000000; i++) {
		__asm__ volatile ("");
	}
	
	printf("Hello world\n");
	
	for (int i = 0; i < 100000000; i++) {
		__asm__ volatile ("");
	}
	
	// TODO: exit correctly
	__asm__ volatile ("int3");
	
	while (1);
}
