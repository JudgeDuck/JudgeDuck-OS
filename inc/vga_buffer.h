#ifndef VGA_BUFFER_H
#define VGA_BUFFER_H

#include <stdint.h>
#include <stddef.h>

namespace vga_buffer {
	enum Color : uint8_t {
		Black = 0, Blue, Green, Cyan,
		Red, Magenta, Brown, LightGray,
		DarkGray, LightBlue, LightGreen, LightCyan,
		LightRed, Pink, Yellow, White,
	};
	
	struct ColorCode {
		uint8_t c;
		
		static ColorCode generate(Color foreground, Color background) {
			return (ColorCode) {(uint8_t) ((background << 4) | foreground)};
		}
		
		ColorCode read() volatile {
			return (ColorCode) {c};
		}
		
		void write(const ColorCode &c) volatile {
			this->c = c.c;
		}
	} __attribute__((packed));
	
	struct ScreenChar {
		uint8_t ascii_character;
		ColorCode color_code;
		
		ScreenChar read() volatile {
			return (ScreenChar) { ascii_character, color_code.read() };
		}
		
		void write(const ScreenChar &s) volatile {
			this->ascii_character = s.ascii_character;
			this->color_code.write(s.color_code);
		}
	} __attribute__((packed));
	
	const size_t BUFFER_HEIGHT = 25;
	const size_t BUFFER_WIDTH = 80;
	
	struct Buffer {
		volatile ScreenChar chars[BUFFER_HEIGHT][BUFFER_WIDTH];
	};
	
	struct Writer {
		size_t column_position;
		ColorCode color_code;
		Buffer *buffer;
		
		void write_byte(uint8_t byte);
		void new_line();
		void clear_row(size_t row);
		
		void write_str(const char *s);
	};
	
	extern Writer *writer;
}

#endif
