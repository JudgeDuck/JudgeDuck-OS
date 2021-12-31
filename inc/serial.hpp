#ifndef SERIAL_H
#define SERIAL_H

#include <stddef.h>

namespace Serial {
    void init();
    void putc(int c);
    void write_buf(const char *s, size_t len);
}

#endif
