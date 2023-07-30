#include <inc/dma.hpp>
#include <inc/logger.hpp>
#include <inc/memory.hpp>
#include <inc/utils.hpp>

using Memory::PAGE_SIZE;

namespace DMA {
    static const size_t DMA_BUFFER_SIZE = 1280 * 1024;
    static volatile char dma_buffer[DMA_BUFFER_SIZE] __attribute__((aligned(PAGE_SIZE)));
    static volatile char *dma_buffer_ptr;
    
    void init() {
        LDEBUG_ENTER_RET();
        dma_buffer_ptr = dma_buffer;
    }
    
    void *alloc(size_t size) {
        LDEBUG_ENTER_RET();
        size = Utils::round_up(size, PAGE_SIZE);
        if (dma_buffer_ptr + size > dma_buffer + DMA_BUFFER_SIZE) {
            LERROR("DMA buffer overflow");
            return nullptr;
        }
        void *ret = (void *) dma_buffer_ptr;
        dma_buffer_ptr += size;
        return ret;
    }
}
