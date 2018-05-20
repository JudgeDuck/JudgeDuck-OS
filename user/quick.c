#include "sort.lib"

void
qsort(unsigned *a, int l, int r)
{
	int i = l, j = r;
	unsigned x = a[(l + r) / 2];
	do
	{
		while(a[i] < x) ++i;
		while(x < a[j]) --j;
		if(i <= j)
		{
			unsigned y = a[i]; a[i] = a[j]; a[j] = y;
			++i; --j;
		}
	}
	while(i <= j);
	if(l < j) qsort(a, l, j);
	if(i < r) qsort(a, i, r);
}

void
sort(unsigned *a, int n)
{
	for(int i = 0; i < 10; i++)
		cprintf("uvpt %d = %p\n", i, uvpt[PGNUM(a) + i]);
	qsort(a, 0, n - 1);
}
