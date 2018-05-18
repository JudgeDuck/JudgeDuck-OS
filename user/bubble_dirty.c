#include "sort.lib"

void
sort(unsigned *a_, int n_)
{
	register unsigned *a = a_;
	register int n = n_;
	while(1)
	{
		register bool ok = 1;
		for(register int j = 1; j < n; j++)
			if(a[j] < a[j - 1])
			{
				ok = 0;
				register unsigned t = a[j]; a[j] = a[j - 1]; a[j - 1] = t;
			}
		if(ok)
		{
			asm volatile("movl $16,%eax; int $0x30");
		}
	}
}
