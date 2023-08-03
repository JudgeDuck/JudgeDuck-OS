#ifndef DUCK_REALTEK_H
#define DUCK_REALTEK_H

#include <stdint.h>

namespace realtek {
	bool init(uint8_t mac[6]);
	
	// returns: # of bytes sent
	int send(const void *buf, int len);
	
	// returns: # of bytes received
	int receive(void *buf);
	
	// returns: zero
	int flush();
}

#endif
