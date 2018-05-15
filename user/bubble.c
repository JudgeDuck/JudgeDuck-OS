#include "sort.lib"

void
sort(unsigned *a, int n)
{
	for(int i = 0; i < n; i++)
		for(int j = 1; j < n; j++)
			if(a[j] < a[j - 1])
			{
				unsigned t = a[j]; a[j] = a[j - 1]; a[j - 1] = t;
			}
}
