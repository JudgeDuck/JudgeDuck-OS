int binary_search(const unsigned *a, int n, unsigned x)
{
	for(int i = 0; i < n; i++)
		if(a[i] == x)
			return i;
	return 233;
}
