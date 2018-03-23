#ifndef JOS_KERN_PMEM_H
#define JOS_KERN_PMEM_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

void pmem_init(void);

#endif	// !JOS_KERN_PMEM_H
