#include <inc/lib.h>
#include <inc/x86.h>

inline unsigned next_int(unsigned x) {
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return x;
}

#define N_qsort 5000000
#define N_radix_sort 50000000

unsigned *sort_a, *sort_b;

unsigned do_run_loop_2e9() {
	for (unsigned i = -2000000000; i; i++) __asm__ volatile ("");
	return 0;
}

void do_qsort(unsigned *a, int l, int r) {
	int i = l, j = r;
	unsigned x = a[(l + r) >> 1];
	do {
		while (a[i] < x) ++i;
		while (x < a[j]) --j;
		if (i <= j) {
			unsigned y = a[i]; a[i] = a[j]; a[j] = y;
			++i; --j;
		}
	} while (i <= j);
	if (l < j) do_qsort(a, l, j);
	if (i < r) do_qsort(a, i, r);
}

unsigned do_run_qsort() {
	unsigned x = 233;
	for (int i = 0; i < N_qsort; i++) {
		x = next_int(x);
		sort_a[i] = x;
	}
	do_qsort(sort_a, 0, N_qsort - 1);
	return sort_a[0];
}

void do_radix_sort(unsigned *a, int n, unsigned *b) {
	static unsigned cnt1[256], cnt2[256], cnt3[256], cnt4[256];
	memset(cnt1, 0, sizeof(cnt1));
	memset(cnt2, 0, sizeof(cnt2));
	memset(cnt3, 0, sizeof(cnt3));
	memset(cnt4, 0, sizeof(cnt4));
	
	for (int i = 0; i < n; i++) {
		cnt1[a[i] & 255]++;
		cnt2[(a[i] >> 8) & 255]++;
		cnt3[(a[i] >> 16) & 255]++;
		cnt4[a[i] >> 24]++;
	}
	
	for (int i = 1; i < 256; i++) {
		cnt1[i] += cnt1[i - 1];
		cnt2[i] += cnt2[i - 1];
		cnt3[i] += cnt3[i - 1];
		cnt4[i] += cnt4[i - 1];
	}
	
	for (int i = n - 1; i >= 0; i--) b[--cnt1[a[i] & 255]] = a[i];
	for (int i = n - 1; i >= 0; i--) a[--cnt2[(b[i] >> 8) & 255]] = b[i];
	for (int i = n - 1; i >= 0; i--) b[--cnt3[(a[i] >> 16) & 255]] = a[i];
	for (int i = n - 1; i >= 0; i--) a[--cnt4[b[i] >> 24]] = b[i];
}

unsigned do_run_radix_sort() {
	unsigned x = 233;
	for (int i = 0; i < N_radix_sort; i++) {
		x = next_int(x);
		sort_a[i] = x;
	}
	do_radix_sort(sort_a, N_radix_sort, sort_b);
	return sort_a[0];
}

extern uint64_t tsc_freq;

uint32_t do_measure(unsigned (*fn)(), const char *name) {
	uint64_t tsc1 = read_tsc();
	unsigned ret = fn();
	uint64_t tsc2 = read_tsc();
	uint64_t tsc_diff = tsc2 - tsc1;
	uint32_t t = tsc_diff * 1000 / tsc_freq;
	
	cprintf("%s: %u M cycles, %u ms, ret %u\n", name, (uint32_t) (tsc_diff >> 20), t, ret);
	
	return t;
}

void do_performance_measurements(uint32_t *t_store) {
	int alloc_size = ROUNDUP(N_radix_sort * 4, PGSIZE);
	void *alloc_end = (void *) 0xc0000000;
	sort_a = (unsigned *) ((char *) alloc_end - alloc_size);
	sort_b = (unsigned *) ((char *) sort_a - alloc_size);
	for (char *p = (char *) sort_b; p < (char *) alloc_end; p += PGSIZE) {
		if (sys_page_alloc(0, p, PTE_P | PTE_U | PTE_W) < 0) {
			cprintf("GG\n");
			sys_reboot();
		}
	}
	
	t_store[0] = do_measure(do_run_loop_2e9, "loop 2e9");
	t_store[1] = do_measure(do_run_qsort, "qsort 5e6");
	t_store[2] = do_measure(do_run_radix_sort, "radix sort 5e7");
	
	for (char *p = (char *) sort_b; p < (char *) alloc_end; p += PGSIZE) {
		if (sys_page_unmap(0, p) < 0) {
			cprintf("GG\n");
			sys_reboot();
		}
	}
}
