#include <stdio.h>
#include <string.h>

char a[3 << 20];
int b[] = {1, 2, 3, 4, 5};
const int c[] = {5, 4, 3, 2, 1};

int main() {
	for (int i = 0; i < 1000000000; i++) {
		__asm__ volatile ("");
	}
	
	printf("Hello world\n");
	memset(a, 0, sizeof(a));
	
	for (int i = 0; i < 1000000000; i++) {
		__asm__ volatile ("");
	}
	
	return 123;
}
