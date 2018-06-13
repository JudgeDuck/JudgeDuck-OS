#include <inc/lib.h>

#define N 10

struct Data
{
	int a[N], b[N], c[N];
};
#define MAX_MEM_KB (1 * 1024)

int plus(int a, int b);

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
std(struct Data *d)
{
	for(int i = 0; i < N; i++)
		d->c[i] = d->a[i] + d->b[i];
}

static void
plus_wrapper(void)
{
	for(int i = 0; i < N; i++)
		u.d.c[i] = plus(u.d.a[i], u.d.b[i]);
	sys_quit_judge();
}

static int
rand(unsigned *seed)
{
	for(;;)
	{
		*seed = *seed * 1103515245u + 12345u;
		int cur = *seed;
		if(cur >= -1000000000 && cur <= 1000000000)
			return cur;
	}
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
	
	unsigned seed = 998244353;
	
	struct Data *d = tasklib_malloc(sizeof(*d));
	for(int i = 0; i < N; i++)
	{
		d->a[i] = rand(&seed);
		d->b[i] = rand(&seed);
	}
	memcpy(&u, d, sizeof(*d));
	
	sys_enter_judge(plus_wrapper, &prm);
	
	std(d);
	
	int fd = open("arbiter.in", O_RDWR | O_CREAT | O_TRUNC);
	for(int i = 0; i < N; i++)
		if(u.d.c[i] != d->c[i])
		{
			fprintf(fd, "Wrong Answer at %d\n", i);
			return;
		}
	fprintf(fd, "Correct Answer!\n");
	close(fd);
}
