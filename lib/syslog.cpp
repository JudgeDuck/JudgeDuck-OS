#include <inc/syslog.hpp>

namespace SysLog {
	static const int MAX_LOG_LEN = 32768;
	
	static char buffer[MAX_LOG_LEN];
	static int head, tail;
	
	void init() {
		head = 0;
		tail = 0;
	}
	
	void putchar(char c) {
		buffer[tail] = c;
		tail = (tail + 1) % MAX_LOG_LEN;
		if (tail == head) {
			head = (head + 1) % MAX_LOG_LEN;
		}
	}
	
	int get(int start, int len, char *buf) {
		int buf_len = (tail - head + MAX_LOG_LEN) % MAX_LOG_LEN;
		if (start < 0) {
			start = buf_len + start;
		}
		if (start < 0) {
			start = 0;
		}
		if (start > buf_len) {
			start = buf_len;
		}
		if (len > buf_len - start) {
			len = buf_len - start;
		}
		
		for (int i = 0; i < len; i++) {
			buf[i] = buffer[(head + start + i) % MAX_LOG_LEN];
		}
		
		return len;
	}
}
