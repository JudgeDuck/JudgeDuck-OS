#include <inc/lib.h>

#define N 10000000
#define M 100

struct Data
{
	const int *a, *b, *c;
	int k[M], ans[M];
};
#define MAX_MEM_KB (640 * 1024)

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

// 这是 WC 2017 挑战 的 标程
static void sort(int *a, int n){
    const int S = 8;
    int _b[10000005], *b;
    int cnt[256];
    int i, j;
    b = _b;
    for (i = 0; i < 32; i += S) {
        for (int j = 0; j < 256; j++) cnt[j] = 0;
        if (i + S >= 32) {
            for (j = 0; j < n; j++) ++cnt[a[j] >> i];
            for (j = 1; j < 1 << S; j++) cnt[j] += cnt[j - 1];
            for (j = n - 1; j >= 0; j--) b[--cnt[a[j] >> i]] = a[j];
            int *tmp = a;
            a = b, b = tmp;
            continue;
        }
        int t = (1 << S) - 1;
        for (j = 0; j < n; j++) ++cnt[(a[j] >> i) & t];
        for (j = 1; j < 1 << S; j++) cnt[j] += cnt[j - 1];
        for (j = n - 1; j >= 0; j--) b[--cnt[(a[j] >> i) & t]] = a[j];
        int *tmp = a;
        a = b, b = tmp;
    }

}

static int std_query_kth(const int *a, int n_a, const int *b, int n_b, const int *c, int n_c, int k)
{
#define get_a(x) ((x) < N ? a[x] : 2000000000)
#define get_b(x) ((x) < N ? b[x] : 2000000000)
#define get_c(x) ((x) < N ? c[x] : 2000000000)
	int ap = 0, bp = 0, cp = 0;
	while (k >= 3)
	{
		int l = k / 3;
		int ta = get_a(ap + l - 1), tb = get_b(bp + l - 1), tc = get_c(cp + l - 1);
		if (ta < tb && ta < tc)
			ap += l;
		else if (tb < tc)
			bp += l;
		else
			cp += l;
		k -= l;
	}

	int t_n = 0;
	int t[6];
	for (int i = 0; i < k; i++)
		t[t_n++] = get_a(ap + i);
	for (int i = 0; i < k; i++)
		t[t_n++] = get_b(bp + i);
	for (int i = 0; i < k; i++)
		t[t_n++] = get_c(cp + i);
	for (int i = 0; i < 6; i++)
		for (int j = 1; j < 6; j++)
			if(t[j] < t[j - 1])
			{
				int u = t[j]; t[j] = t[j - 1]; t[j - 1] = u;
			}
	return t[k - 1];
}

static void
std(struct Data *d)
{
	for(int i = 0; i < M; i++)
		d->ans[i] = std_query_kth(d->a, N, d->b, N, d->c, N, d->k[i]);
}

int query_kth(const int *a, int n_a, const int *b, int n_b, const int *c, int n_c, int k);

static void
contestant_wrapper(void)
{
	for(int i = 0; i < M; i++)
		u.d.ans[i] = query_kth(u.d.a, N, u.d.b, N, u.d.c, N, u.d.k[i]);
	sys_quit_judge();
}

static int
rand(unsigned *seed)
{
	return (*seed = *seed * 1103515245u + 12345u) % 1000000000u + 1u;
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
	int *a = tasklib_malloc(sizeof(int) * N);
	int *b = tasklib_malloc(sizeof(int) * N);
	int *c = tasklib_malloc(sizeof(int) * N);
	for(int i = 0; i < N; i++) a[i] = rand(&seed);
	for(int i = 0; i < N; i++) b[i] = rand(&seed);
	for(int i = 0; i < N; i++) c[i] = rand(&seed);
	void *old_stack_ptr = 0;
	void *new_stack_ptr = &u + 1;
	asm volatile("xchg %%esp, %0\n\t"
        : "=r"(old_stack_ptr)
        : "0"(new_stack_ptr)
    );
	sort(a, N);
	sort(b, N);
	sort(c, N);
	asm volatile("xchg %%esp, %0\n\t"
        : "=r"(new_stack_ptr)
        : "0"(old_stack_ptr)
    );
	
	struct Data *d = tasklib_malloc(sizeof(*d));
	d->a = a;
	d->b = b;
	d->c = c;
	for(int i = 0; i < M; i++) d->k[i] = rand(&seed) % (3 * N) + 1;
	
	memcpy(&u, d, sizeof(*d));
	
	sys_enter_judge(contestant_wrapper, &prm);
	
	old_stack_ptr = 0;
	new_stack_ptr = &u + 1;
	asm volatile("xchg %%esp, %0\n\t"
        : "=r"(old_stack_ptr)
        : "0"(new_stack_ptr)
    );
	std(d);
	asm volatile("xchg %%esp, %0\n\t"
        : "=r"(new_stack_ptr)
        : "0"(old_stack_ptr)
    );
	
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
