#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

#include <inc/vga_buffer.hpp>

namespace FrameBuffer {
	void init();
	
	void set_framebuffer_addr(uint64_t framebuffer_addr);
	
	void write_byte(uint8_t byte, VGA_Buffer::ColorCode color_code);
}


#endif
