#ifndef DUCK_DMA_H
#define DUCK_DMA_H

#include <stddef.h>

namespace DMA {
    void init();
    void *alloc(size_t size);
}

#endif
