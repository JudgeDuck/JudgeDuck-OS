#include <inc/vga_buffer.hpp>
#include <inc/framebuffer.hpp>

namespace VGA_Buffer {
	Writer the_writer = (Writer) {
		.column_position = 0,
		.color_code = ColorCode::generate(Color::LightGray, Color::Black),
		.buffer = (VGA_Buffer::Buffer *) 0xb8000,
	};
	Writer *writer = &the_writer;
	
	void Writer::write_byte(uint8_t byte) {
		FrameBuffer::write_byte(byte, this->color_code);
		
		if (byte == '\n') {
			this->new_line();
		} else {
			if (this->column_position >= BUFFER_WIDTH) {
				this->new_line();
			}
			
			const auto row = BUFFER_HEIGHT - 1;
			const auto col = this->column_position;
			
			const auto color_code = this->color_code;
			this->buffer->chars[row][col].write((ScreenChar) {
				.ascii_character = byte,
				.color_code = color_code,
			});
			
			this->column_position += 1;
		}
	}
	
	void Writer::new_line() {
		for (size_t row = 1; row < BUFFER_HEIGHT; row++) {
			for (size_t col = 0; col < BUFFER_WIDTH; col++) {
				ScreenChar s = buffer->chars[row][col].read();
				buffer->chars[row - 1][col].write(s);
			}
		}
		this->clear_row(BUFFER_HEIGHT - 1);
		this->column_position = 0;
	}
	
	void Writer::clear_row(size_t row) {
		const auto blank = (ScreenChar) {
			.ascii_character = ' ',
			.color_code = this->color_code,
		};
		for (size_t col = 0; col < BUFFER_WIDTH; col++) {
			buffer->chars[row][col].write(blank);
		}
	}
	
	void Writer::write_str(const char *s) {
		while (*s) {
			this->write_byte(*s);
			++s;
		}
	}
	
	void Writer::write_buf(const char *s, size_t len) {
		for (size_t i = 0; i != len; i++) {
			this->write_byte(*s);
			++s;
		}
	}
}
