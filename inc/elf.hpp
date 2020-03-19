#ifndef DUCK_ELF_H
#define DUCK_ELF_H

namespace ELF {
	struct App64 {
		uint64_t entry;
		uint64_t rsp;
	};
	
	bool load(const char *buf, uint32_t len, void *dst, App64 &app);
	void run(const App64 &app);
}

#endif
