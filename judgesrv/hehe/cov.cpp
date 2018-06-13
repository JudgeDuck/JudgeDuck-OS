#include <cstdio>

char buf[233333];

int main()
{
	gets(buf);
	puts(buf);
	printf("#include <inc/lib.h>\n");
	printf("void sort(unsigned *a, int n){\n");
	while(gets(buf)) puts(buf);
	printf("\n}\n");
	return 0;
}
