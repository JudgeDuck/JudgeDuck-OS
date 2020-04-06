#ifndef DUCK_E1000_H
#define DUCK_E1000_H

namespace e1000 {
	bool init(uint8_t mac[6]);
	
	// returns: # of bytes sent
	int send(const void *buf, int len);
	
	// returns: # of bytes received
	int receive(void *buf);
	
	// returns: zero
	int flush();
}

#endif
