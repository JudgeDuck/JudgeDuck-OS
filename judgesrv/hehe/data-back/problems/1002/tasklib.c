#include <inc/lib.h>
#include <math.h>

#define M_PI 3.141592653589793238

struct Data
{
	unsigned a[10000007];
	unsigned b[10000007];
	unsigned c[20000007];
	int n, m;
};
#define MAX_MEM_KB (512 * 1024)

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

typedef struct
{
	double real, imag;
} complex;
static complex complex_add(complex a, complex b)
{
	return (complex)
	{
		a.real + b.real, a.imag + b.imag
	};
}
static complex complex_sub(complex a, complex b)
{
	return (complex)
	{
		a.real - b.real, a.imag - b.imag
	};
}
static complex complex_mul(complex a, complex b)
{
	return (complex)
	{
		a.real * b.real - a.imag * b.imag,
			   a.real * b.imag + a.imag * b.real
	};
}
static void fft(complex *a, int n, int flag)
{
	int i, j, k;
	complex w, u, v, root;
	for (i = n / 2, j = 1; j < n; ++j)
	{
		if (i < j)
		{
			w = a[i];
			a[i] = a[j];
			a[j] = w;
		}
		k = n / 2;
		while (i & k)
		{
			i ^= k;
			k /= 2;
		}
		i ^= k;
	}
	for (k = 2; k <= n; k *= 2)
	{
		root = (complex)
		{
			cos(M_PI / k * flag * 2), sin(M_PI / k * flag * 2)
		};
		for (i = 0; i < n; i += k)
		{
			w = (complex)
			{
				1.0, 0.0
			};
			for (j = i; j < i + k / 2; ++j)
			{
				u = a[j];
				v = complex_mul(a[j + k / 2], w);
				a[j] = complex_add(u, v);
				a[j + k / 2] = complex_sub(u, v);
				w = complex_mul(w, root);
			}
		}
	}
}
static void std(unsigned *a_, int n, unsigned *b_, int m, unsigned *c)
{
	++n; ++m;
	complex *a = tasklib_malloc(sizeof(complex) * (33554432));
	complex *b = tasklib_malloc(sizeof(complex) * (33554432));
	for(int i = 0; i < n; i++) a[i].real = a_[i];
	for(int i = 0; i < m; i++) b[i].real = b_[i];
	int len = 2;
	while(len < m + n) len <<= 1;
	fft(a, len, 1);
	fft(b, len, 1);
	for(int i = 0; i < len; ++i) a[i] = complex_mul(a[i], b[i]);
	fft(a, len, -1);
	for(int i = 0; i < n + m - 1; ++i) c[i] = (unsigned)(a[i].real + 0.5);
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
	d->n = 10000000;
	d->m = 10000000;
	for(int i = 0; i < d->n; i++)
		d->a[i] = rand(&seed);
	for(int i = 0; i < d->m; i++)
		d->b[i] = rand(&seed);
	memcpy(&u, d, sizeof(*d));
	sys_enter_judge(poly_multiply_wrapper, &prm);
	
	std(d->a, d->n, d->b, d->m, d->c);
	
	int fd = open("arbiter.in", O_RDWR | O_CREAT | O_TRUNC);
	for(int i = 0; i < d->n + d->m; i++)
		if(u.d.c[i] != d->c[i])
		{
			fprintf(fd, "Wrong Answer at %d\n", i);
			return;
		}
	fprintf(fd, "Correct Answer!\n");
	close(fd);
}
