#ifndef JOS_INC_MALLOC_H
#define JOS_INC_MALLOC_H 1

#ifdef __cplusplus
extern "C" {
#endif

void *malloc(size_t size);
void free(void *addr);

#ifdef __cplusplus
}
#endif

#endif
