#include <inc/lib.h>
#include <math.h>

#define M_PI 3.141592653589793238

struct Data
{
	unsigned a[1000007];
	unsigned b[1000007];
	unsigned c[2000007];
	int n, m;
};
#define MAX_MEM_KB (960 * 1024)

void poly_multiply(unsigned *a, int n, unsigned *b, int m, unsigned *c);

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
poly_multiply_wrapper(void)
{
	poly_multiply(u.d.a, u.d.n, u.d.b, u.d.m, u.d.c);
	sys_quit_judge();
}

static int
rand(unsigned *seed)
{
	*seed = *seed * 1103515245u + 12345u;
	return *seed % 10;
}

void
umain(int argc, char **argv)
{
	if(argc != 3) return;
	
	int time_ms = atoi(argv[1]);
	int mem_kb = atoi(argv[2]);
	
	if(mem_kb > MAX_MEM_KB) return;
	
	struct JudgeParams prm;
	memset(&prm, 0, sizeof(prm));
	prm.ms = time_ms;
	prm.kb = mem_kb;
	prm.syscall_enabled[SYS_quit_judge] = 1;
	
	unsigned seed = 23456789;
	
	struct Data *d = tasklib_malloc(sizeof(*d));
	d->n = 1000000;
	d->m = 1000000;
	long long jha = 0; int j = 0;
	jha ^= (long long) j++ * 1000000;
	jha ^= (long long) j++ * 1000000;
	for(int i = 0; i <= d->n; i++)
	{
		d->a[i] = rand(&seed);
		jha ^= (long long) j++ * d->a[i];
	}
	for(int i = 0; i <= d->m; i++)
	{
		d->b[i] = rand(&seed);
		jha ^= (long long) j++ * d->b[i];
	}
	memcpy(&u, d, sizeof(*d));
	sys_enter_judge(poly_multiply_wrapper, &prm);
	
	long long ha = 0;
	
	int fd = open("arbiter.in", O_RDWR | O_CREAT | O_TRUNC);
	for(int i = 0; i <= d->n + d->m; i++)
		ha ^= (long long) (i + 1) * u.d.c[i];
	if(ha != 3356935165808ll)
	{
		fprintf(fd, "Wrong Answer\n");
		return;
	}
	fprintf(fd, "Correct Answer!\n");
	close(fd);
}
