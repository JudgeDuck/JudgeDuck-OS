#ifndef DUCK_SYSLOG_H
#define DUCK_SYSLOG_H

namespace SysLog {
	void init();
	void putchar(char c);
	int get(int start, int len, char *buf);
}

#endif
