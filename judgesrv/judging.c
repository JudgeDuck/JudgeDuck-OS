#include "sort.lib"
void sort(unsigned *a, int n) {

#define N 50000
#define D 256
#define D1 255
#define clr_t for(i=0;i<D;++i)t[i]=0
#define cal_rs for(i=0;i<D;++i)rs[i]=p,p+=t[i]
	unsigned b[N],*rs[D],t[D],*p,i;
	
	clr_t;
	for(i=0;i<N;++i)++t[a[i]&D1];
	p=b;cal_rs;
	for(i=0;i<N;++i)*rs[a[i]&D1]++=a[i];
	
	clr_t;
	for(i=0;i<N;++i)++t[b[i]>>8&D1];
	p=a;cal_rs;
	for(i=0;i<N;++i)*rs[b[i]>>8&D1]++=b[i];
	
	clr_t;
	for(i=0;i<N;++i)++t[a[i]>>16&D1];
	p=b;cal_rs;
	for(i=0;i<N;++i)*rs[a[i]>>16&D1]++=a[i];
	
	clr_t;
	for(i=0;i<N;++i)++t[b[i]>>24];
	p=a;cal_rs;
	for(i=0;i<N;++i)*rs[b[i]>>24]++=b[i];
}
