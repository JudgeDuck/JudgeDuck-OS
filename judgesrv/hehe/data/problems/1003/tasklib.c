#include <inc/lib.h>

#define N 100000000
#define M 100

struct Data
{
	const unsigned *a;
	unsigned x[M], ans[M];
};
#define MAX_MEM_KB (404 * 1024)

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

static int fake_binary_search(const unsigned *a, int n, unsigned x)
{
	int l = 0, r = n - 1;
	while(l < r)
	{
		int mid = (l + r) >> 1;
		if(a[mid] < x) l = mid + 1; else r = mid;
	}
	return l;
}

static void
std(struct Data *d)
{
	for(int i = 0; i < M; i++)
		d->ans[i] = fake_binary_search(d->a, N, d->x[i]);
}

int binary_search(const unsigned *a, int n, unsigned x);

static void
binary_search_wrapper(void)
{
	for(int i = 0; i < M; i++)
		u.d.ans[i] = binary_search(u.d.a, N, u.d.x[i]);
	sys_quit_judge();
}

// 这是 WC 2017 挑战 的 标程
static void sort(unsigned *a, int n){
    const int S = 8;
    unsigned _b[100000005], *b;
    unsigned cnt[256];
    int i, j;
    b = _b;
    for (i = 0; i < 32; i += S) {
        for (int j = 0; j < 256; j++) cnt[j] = 0;
        if (i + S >= 32) {
            for (j = 0; j < n; j++) ++cnt[a[j] >> i];
            for (j = 1; j < 1 << S; j++) cnt[j] += cnt[j - 1];
            for (j = n - 1; j >= 0; j--) b[--cnt[a[j] >> i]] = a[j];
            unsigned *tmp = a;
            a = b, b = tmp;
            continue;
        }
        unsigned t = (1 << S) - 1;
        for (j = 0; j < n; j++) ++cnt[(a[j] >> i) & t];
        for (j = 1; j < 1 << S; j++) cnt[j] += cnt[j - 1];
        for (j = n - 1; j >= 0; j--) b[--cnt[(a[j] >> i) & t]] = a[j];
        unsigned *tmp = a;
        a = b, b = tmp;
    }

}

static unsigned
rand(unsigned *seed)
{
	return *seed = *seed * 1103515245u + 12345u;
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
	unsigned *a = tasklib_malloc(sizeof(unsigned) * N);
	for(int i = 0; i < N; i++) a[i] = rand(&seed);
	void *old_stack_ptr = 0;
	void *new_stack_ptr = &u + 1;
	asm volatile("xchg %%esp, %0\n\t"
        : "=r"(old_stack_ptr)
        : "0"(new_stack_ptr)
    );
	sort(a, N);
	asm volatile("xchg %%esp, %0\n\t"
        : "=r"(new_stack_ptr)
        : "0"(old_stack_ptr)
    );
	
	struct Data *d = tasklib_malloc(sizeof(*d));
	d->a = a;
	for(int i = 0; i < M; i++) d->x[i] = a[rand(&seed) % N];
	
	memcpy(&u, d, sizeof(*d));
	
	sys_enter_judge(binary_search_wrapper, &prm);
	
	std(d);
	
	int fd = open("arbiter.in", O_RDWR | O_CREAT | O_TRUNC);
	for(int i = 0; i < M; i++)
		if(u.d.ans[i] != d->ans[i])
		{
			fprintf(fd, "Wrong Answer at %d\n", i);
			return;
		}
	fprintf(fd, "Correct Answer!\n");
	close(fd);
}
