#include "sort.lib"

void sort(unsigned *a, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			if (a[j] < a[i]) {
				int t = a[i];
				a[i] = a[j];
				a[j] = t;
			}
		}
	}
}
