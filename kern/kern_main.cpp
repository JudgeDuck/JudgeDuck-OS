#include <inc/multiboot2_loader.h>
#include <stdio.h>
#include <math.h>

#include <algorithm>
#include <vector>
#include <exception>
#include <iostream>

static void print_hello() {
	printf("Hello world!\n");
	
	double e = 0, tmp = 1;
	for (int i = 1; i <= 20; i++) {
		e += tmp;
		tmp /= i;
	}
	printf("e = %.15lf\n", e);
	printf("pi = 2 * atan2(1, 0) = %.15lf\n", 2 * atan2(1, 0));
	
	std::vector<int> a = {3, 5, 2, 4, 1};
	std::sort(a.begin(), a.end());
	try {
		int a[5] = {3, 5, 2, 4, 1};
		std::sort(a, a + 5);
		for (int i = 0; i < 5; i++) {
			printf("%d%c", a[i], " \n" [i == 5 - 1]);
		}
	} catch (std::exception &e) {
		
	}
	
	std::cout << "std::cout works!" << std::endl;
}

int main() {
	print_hello();
	
	multiboot2_loader::load();
	
	while (1);
}
