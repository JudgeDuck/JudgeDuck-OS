#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <inc/duck_server.hpp>
#include <inc/network_driver.hpp>
#include <inc/timer.hpp>
#include <inc/logger.hpp>
#include <inc/utils.hpp>
#include <inc/x86_64.hpp>
#include <inc/scheduler.hpp>
#include <inc/judger.hpp>
#include <ducknet.h>

using NetworkDriver::mac;
using NetworkDriver::ip;

using std::min;

namespace DuckServer {
	static DucknetMACAddress my_mac;
	static DucknetIPv4Address my_ip;
	
	const uint16_t DUCK_UDP_PORT = 9000;
	
	static void GG_reboot() {
		LDEBUG("GG, reboot");
		LINFO("GG, reboot");
		LWARN("GG, reboot");
		LERROR("GG, reboot");
		LFATAL("GG, reboot");
		Timer::microdelay((uint64_t) 3e6);  // 3s
		x86_64::reboot();
	}
	
	static inline bool starts_with(const char *s1, int len1, const char *s2) {
		int len2 = strlen(s2);
		return len1 >= len2 && memcmp(s1, s2, len2) == 0;
	}
	
	static inline bool equals_to(const char *s1, int len1, const char *s2) {
		int len2 = strlen(s2);
		return len1 == len2 && memcmp(s1, s2, len2) == 0;
	}
	
	static bool process_judge_control(char *content, int len, char *&res, int &res_len) {
		uint64_t in_size;
		Judger::JudgeConfig conf;
		
		res = NULL;
		res_len = -1;
		content[len] = 0;
		
		if (1 == sscanf(content, "set-elf-size %lu", &in_size)) {
			if (Judger::set_elf_size(in_size)) {
				res = content;
				strcpy(res, "ok-set-elf-size");
			}
		} else if (1 == sscanf(content, "set-stdin-size %lu", &in_size)) {
			if (Judger::set_stdin_size(in_size)) {
				res = content;
				strcpy(res, "ok-set-stdin-size");
			}
		} else if (5 == sscanf(content, "judge %lu %lu %lu %lu %lu",
			&conf.time_limit_ns, &conf.memory_hard_limit_kb,
			&conf.stdout_max_size, &conf.stderr_max_size, &conf.seq_num)) {
			static char res_str[1024];
			
			auto j_res = Judger::judge(conf);
			if (j_res.error) {
				res = res_str;
				res_len = snprintf(res_str, sizeof(res_str),
					"ok-judge\nJudge Failed\n%s\n", j_res.error);
			} else {
				const char *RE_STR = "Runtime Error";
				const char *TLE_STR = "Time Limit Exceeded";
				const char *FINISH_STR = "Run Finished";
				res = res_str;
				res_len = snprintf(res_str, sizeof(res_str),
					"ok-judge\n"
					"time-ns %lu\n"
					"time-tsc %lu\n"
					"memory-kb %lu\n"
					"stdout-size %lu\n"
					"stderr-size %lu\n"
					"return-code %d\n"
					"status %s\n",
					j_res.time_ns, j_res.time_tsc, j_res.memory_kb,
					j_res.stdout_size, j_res.stderr_size, j_res.return_code,
					j_res.is_RE ? RE_STR : j_res.is_TLE ? TLE_STR : FINISH_STR
				);
			}
		} else {
			return false;
		}
		
		if (res && res_len == -1) {
			res_len = strlen(res);
		}
		return true;
	}
	
	static bool process_judge_data(char *content, int len, char *&res, int &res_len) {
		uint64_t query_off, query_len;
		static char query_res[2048];
		const int MAX_QUERY_LEN = 1400;
		
		res = NULL;
		res_len = -1;
		content[len] = 0;
		
		if (starts_with(content, len, "put-elf-data")) {
			uint64_t tmp_len = strlen("put-elf-data");
			char *data = content + tmp_len;
			uint64_t data_len = len - tmp_len;
			if (data_len >= 8) {
				query_off = * (uint64_t *) data;
				query_len = data_len - 8;
				if (Judger::put_elf_data(query_off, data + 8, query_len)) {
					res = content;
					sprintf(res, "ack-elf-data %lu", query_off);
				}
			}
		} else if (starts_with(content, len, "put-stdin-data")) {
			uint64_t tmp_len = strlen("put-stdin-data");
			char *data = content + tmp_len;
			uint64_t data_len = len - tmp_len;
			if (data_len >= 8) {
				query_off = * (uint64_t *) data;
				query_len = data_len - 8;
				if (Judger::put_stdin_data(query_off, data + 8, query_len)) {
					res = content;
					sprintf(res, "ack-stdin-data %lu", query_off);
				}
			}
		} else if (2 == sscanf(content, "get-stdout-data %lu %lu", &query_off, &query_len)) {
			if (query_len <= MAX_QUERY_LEN) {
				int query_res_len;
				if (Judger::get_stdout_data(query_off, query_len, query_res, query_res_len)) {
					res = content;
					int tmp_len = sprintf(res, "stdout-data");
					* (uint64_t *) (res + tmp_len) = query_off;
					memcpy(res + tmp_len + 8, query_res, query_res_len);
					res_len = tmp_len + 8 + query_res_len;
				}
			}
		} else if (2 == sscanf(content, "get-stderr-data %lu %lu", &query_off, &query_len)) {
			if (query_len <= MAX_QUERY_LEN) {
				int query_res_len;
				if (Judger::get_stderr_data(query_off, query_len, query_res, query_res_len)) {
					res = content;
					int tmp_len = sprintf(res, "stderr-data");
					* (uint64_t *) (res + tmp_len) = query_off;
					memcpy(res + tmp_len + 8, query_res, query_res_len);
					res_len = tmp_len + 8 + query_res_len;
				}
			}
		} else {
			return false;
		}
		
		if (res && res_len == -1) {
			res_len = strlen(res);
		}
		return true;
	}
	
	static int duck_packet_handle(DucknetIPv4Address src,
		uint16_t sport, char *content, int content_len) {
		// TODO: handle seq-num
		
		if (equals_to(content, content_len, "reboot")) {
			GG_reboot();
		} else {
			char *to_send = NULL;
			int to_send_len = 0;
			bool success = false;
			success = process_judge_control(content, content_len, to_send, to_send_len);
			if (!success) {
				process_judge_data(content, content_len, to_send, to_send_len);
			}
			
			if (to_send) {
				ducknet_udp_send(src, sport, DUCK_UDP_PORT, to_send, to_send_len);
			}
		}
		
		return -1;
	}
	
	static int udp_packet_handle(DucknetIPv4Address src,
		DucknetIPv4Address dst, DucknetUDPHeader *hdr, int content_len) {
		// Drop unwanted packets
		if (src.a[0] == 255) return -1;
		if (dst.addr != my_ip.addr) return -1;
		
		char *content = (char *) (hdr + 1);
		
		if (hdr->dport == DUCK_UDP_PORT) {
			return duck_packet_handle(src, hdr->sport, content, content_len);
		}
		
		return -1;
	}
	
	static int phy_recv_packet_handle(void *, int) {
		Scheduler::set_active();
		return 0;
	}
	
	static int phy_send_packet_handle(const void *, int) {
		Scheduler::set_active();
		return 0;
	}
	
	static int idle() {
		Scheduler::set_idle();
		
		if (Scheduler::can_sleep()) {
			ducknet_flush();
			Scheduler::sleep();
		}
		
		return 0;
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
				.packet_handle = phy_recv_packet_handle,
				.send_packet_handle = phy_send_packet_handle,
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
			.idle = idle,
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
