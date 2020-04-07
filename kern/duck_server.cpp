#include <string.h>

#include <inc/duck_server.hpp>
#include <inc/network_driver.hpp>
#include <inc/timer.hpp>
#include <inc/logger.hpp>
#include <inc/utils.hpp>
#include <inc/x86_64.hpp>
#include <ducknet.h>

using NetworkDriver::mac;
using NetworkDriver::ip;

namespace DuckServer {
	static DucknetMACAddress my_mac;
	static DucknetIPv4Address my_ip;
	
	static void GG_reboot() {
		LDEBUG("GG, reboot");
		LINFO("GG, reboot");
		LWARN("GG, reboot");
		LERROR("GG, reboot");
		LFATAL("GG, reboot");
		Timer::microdelay((uint64_t) 3e6);  // 3s
		x86_64::reboot();
	}
	
	static int udp_packet_handle(DucknetIPv4Address src,
		DucknetIPv4Address dst, DucknetUDPHeader *hdr, int content_len) {
		// Drop unwanted packets
		if (src.a[0] == 255) return -1;
		if (dst.addr != my_ip.addr) return -1;
		
		char *content = (char *) (hdr + 1);
		if (strncmp(content, "reboot", content_len) == 0) {
			GG_reboot();
		}
		
		return -1;
	}
	
	void init() {
		LDEBUG_ENTER_RET();
		
		my_mac = (DucknetMACAddress) {
			.a = { mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] }
		};
		my_ip = (DucknetIPv4Address) {
			.a = { ip[3], ip[2], ip[1], ip[0] }
		};
		
		DucknetConfig conf = {
			.utils = {
				.tsc_freq = Timer::tsc_freq,
			},
			.phy = {
				.send = NetworkDriver::send,
				.recv = NetworkDriver::receive,
				.flush = NetworkDriver::flush,
				.packet_handle = NULL,
			},
			.ether = {
				.mac = my_mac,
				.packet_handle = NULL,
			},
			.arp = {
				.packet_handle = NULL,
			},
			.ipv4 = {
				.ip = my_ip,
				.packet_handle = NULL,
			},
			.icmp = {
				.packet_handle = NULL,
			},
			.udp = {
				.packet_handle = udp_packet_handle,
			},
			.idle = NULL,
		};
		
		if (ducknet_init(&conf) < 0) {
			LFATAL("ducknet init failed");
		}
	}
	
	void run() {
		LDEBUG_ENTER_RET();
		
		LINFO("Starting duck server");
		
		ducknet_mainloop();
	}
}
