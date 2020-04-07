#include <string.h>

#include <inc/network_driver.hpp>
#include <inc/pci.hpp>
#include <inc/logger.hpp>
#include <inc/e1000.hpp>
#include <inc/multiboot2_loader.hpp>

namespace NetworkDriver {
	static bool read_ip_mac(const char *cmdline, uint8_t ip[4], uint8_t mac[6]) {
		const int MAX_LEN = 256;
		char ip_str[MAX_LEN], mac_str[MAX_LEN];
		
		int r = sscanf(cmdline, " ip = %s mac = %s ", ip_str, mac_str);
		if (r != 2) return false;
		
		r = sscanf(ip_str, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
		if (r != 4) return false;
		
		r = sscanf(mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
		if (r != 6) return false;
		
		return true;
	}
	
	uint8_t ip[4], mac[6];
	
	// returns: # of bytes sent
	int (*send)(const void *buf, int len);
	
	// returns: # of bytes received
	int (*receive)(void *buf);
	
	// returns: zero
	int (*flush)();
	
	void init() {
		LDEBUG_ENTER_RET();
		
		if (!read_ip_mac(Multiboot2_Loader::command_line, ip, mac)) {
			LWARN("Invalid ip/mac arguments, using defaults...");
			uint8_t _ip[4] = { 10, 0, 2, 123 };
			uint8_t _mac[6] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 };
			memcpy(ip, _ip, sizeof(ip));
			memcpy(mac, _mac, sizeof(mac));
		}
		
		if (e1000::init(mac)) {
			send = e1000::send;
			receive = e1000::receive;
			flush = e1000::flush;
			LINFO("e1000 driver initialized");
		} else {
			LWARN("No network driver found. Running in standalone mode...");
		}
		
		LINFO("IP = %u.%u.%u.%u  MAC = %02x:%02x:%02x:%02x:%02x:%02x",
			ip[0], ip[1], ip[2], ip[3],
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
}
