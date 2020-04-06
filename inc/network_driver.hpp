#ifndef DUCK_NETWORK_DRIVER_H
#define DUCK_NETWORK_DRIVER_H

#include <stdint.h>

namespace NetworkDriver {
	extern uint8_t ip[4];
	extern uint8_t mac[6];
	
	void init();
}

#endif
