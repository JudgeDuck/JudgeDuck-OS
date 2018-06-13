#include <inc/lib.h>

struct Data
{
	unsigned a[100000007]; int n;
};
#define MAX_MEM_KB (960 * 1024)

void sort(unsigned *a, int n);

static int
atoi(const char *s)
{
	int ans = 0;
	while(*s) ans = ans * 10 + (*s++ - '0');
	return ans;
}
static void *
tasklib_malloc(size_t size)
{
	static void *malloc_end = (void *) 0xd0000000;
	size = ROUNDUP(size, PGSIZE);
	void *ret = malloc_end - size;
	for(void *i = ret; i < malloc_end; i += PGSIZE)
		if(sys_page_alloc(0, i, PTE_P | PTE_U | PTE_W) < 0)
			panic("malloc failed %p\n", i);
	malloc_end = ret;
	return ret;
}

#define u uSDFHSDHFLHWEOHSE
union
{
	char USER_STACK[MAX_MEM_KB << 10];
	struct Data d;
} u __attribute__((aligned(PGSIZE)));
// 仅允许开这一个全局变量！其他的包括static的都不能开

static void
GGGqsort(unsigned *a, int l, int r)
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
	if(l < j) GGGqsort(a, l, j);
	if(i < r) GGGqsort(a, i, r);
}

static void
sort_wrapper(void)
{
	sort(u.d.a, u.d.n);
	sys_quit_judge();
}

void
umain(int argc, char **argv)
{
	if(argc != 3) return;
	
	int time_ms = atoi(argv[1]);
	int mem_kb = atoi(argv[2]);
	int n = 100000000;
	
	if(mem_kb > MAX_MEM_KB) return;
	
	struct JudgeParams prm;
	memset(&prm, 0, sizeof(prm));
	prm.ms = time_ms;
	prm.kb = mem_kb;
	prm.syscall_enabled[SYS_quit_judge] = 1;
	
	struct Data *d = tasklib_malloc(sizeof(*d));
	d->n = n;
	d->a[0] = 998244353u;
	for(int i = 1; i < d->n; i++)
		d->a[i] = d->a[i - 1] * 13131u + 3131313u;
	memcpy(&u, d, sizeof(*d));
	sys_enter_judge(sort_wrapper, &prm);
	
	GGGqsort(d->a, 0, d->n - 1);
	
	int fd = open("arbiter.in", O_RDWR | O_CREAT | O_TRUNC);
	for(int i = 0; i < d->n; i++)
		if(u.d.a[i] != d->a[i])
		{
			fprintf(fd, "Wrong Answer at %d\n", i);
			return;
		}
	fprintf(fd, "Correct Answer!\n");
	close(fd);
}
