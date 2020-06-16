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
	
	const uint16_t DUCK_UDP_PORT = 9999;
	
	static inline bool starts_with(const char *s1, int len1, const char *s2) {
		int len2 = strlen(s2);
		return len1 >= len2 && memcmp(s1, s2, len2) == 0;
	}
	
	static inline bool equals_to(const char *s1, int len1, const char *s2) {
		int len2 = strlen(s2);
		return len1 == len2 && memcmp(s1, s2, len2) == 0;
	}
	
	static bool process_controls(char *content, int len, char *&res, int &res_len) {
		res = NULL;
		res_len = -1;
		content[len] = 0;
		
		if (equals_to(content, len, "uptime")) {
			uint64_t t = (uint64_t) Timer::secf_since_epoch();
			res = content;
			sprintf(res, "uptime %lu", t);
		} else if (equals_to(content, len, "statistics")) {
			auto stat = Judger::get_statistics();
			res = content;
			sprintf(res, "statistics %lu %lu", stat.n_judges, stat.total_time_ns);
		} else {
			return false;
		}
		
		if (res && res_len == -1) {
			res_len = strlen(res);
		}
		return true;
	}
	
	static bool process_data(char *content, int len, char *&res, int &res_len) {
		res = NULL;
		res_len = -1;
		content[len] = 0;
		
		uint64_t q_off, q_off2, q_len;
		const uint64_t MAX_QUERY_LEN = 1400;
		static char q_res[2048];
		
		if (equals_to(content, len, "query-buffer-size")) {
			uint64_t buffer_size = Judger::query_buffer_size();
			res = content;
			sprintf(res, "ok-query-buffer-size %lu", buffer_size);
		} else if (2 == sscanf(content, "clear-buffer %lu %lu", &q_off, &q_len)) {
			if (Judger::clear_buffer(q_off, q_len)) {
				res = content;
				sprintf(res, "ok-clear-buffer %lu %lu", q_off, q_len);
			}
		} else if (2 == sscanf(content, "read-buffer %lu %lu", &q_off, &q_len)) {
			if (q_len > MAX_QUERY_LEN) return true;
			if (Judger::read_buffer(q_off, q_len, q_res)) {
				res = content;
				res_len = sprintf(res, "ok-read-buffer");
				* (uint64_t *) (res + res_len) = q_off;
				res_len += 8;
				memcpy(res + res_len, q_res, q_len);
				res_len += q_len;
			}
		} else if (starts_with(content, len, "write-buffer")) {
			uint64_t tmp_len = strlen("write-buffer");
			char *data = content + tmp_len;
			uint64_t data_len = (uint64_t) len - tmp_len;
			if (data_len < 8) return true;
			q_off = * (uint64_t *) data;
			q_len = data_len - 8;
			if (Judger::write_buffer(q_off, data + 8, q_len)) {
				res = content;
				sprintf(res, "ok-write-buffer %lu %lu", q_off, q_len);
			}
		} else if (3 == sscanf(content, "copy-buffer %lu %lu %lu", &q_off, &q_off2, &q_len)) {
			if (Judger::copy_buffer(q_off, q_off2, q_len)) {
				res = content;
				sprintf(res, "ok-copy-buffer %lu %lu %lu", q_off, q_off2, q_len);
			}
		} else if (3 == sscanf(content, "compare-buffer %lu %lu %lu", &q_off, &q_off2, &q_len)) {
			bool comp_result;
			if (Judger::compare_buffer(q_off, q_off2, q_len, comp_result)) {
				res = content;
				sprintf(res, "ok-compare-buffer %lu %lu %lu %lu",
					q_off, q_off2, q_len, (uint64_t) comp_result);
			}
		} else {
			return false;
		}
		
		if (res && res_len == -1) {
			res_len = strlen(res);
		}
		return true;
	}
	
	static bool process_judge(char *content, int len, char *&res, int &res_len) {
		res = NULL;
		res_len = -1;
		content[len] = 0;
		Judger::JudgeRequest req;
		
		if (11 == sscanf(content, "judge %lu %lu %lu "  // seq, tlns, mhlkb
			"%lu %lu %lu %lu %lu %lu %lu %lu",  // ELF, I, O, E
			&req.seq_num, &req.time_limit_ns, &req.memory_hard_limit_kb,
			&req.ELF.off, &req.ELF.len,
			&req.stdin.off, &req.stdin.len,
			&req.stdout.off, &req.stdout.len,
			&req.stderr.off, &req.stderr.len)) {
			static char res_str[1024];
			
			auto j_res = Judger::judge(req);
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
					"count-inst %lu\n"
					"clk-thread %lu\n"
					"clk-ref-tsc %lu\n"
					"time-ns %lu\n"
					"time-tsc %lu\n"
					"memory-kb %lu\n"
					"stdout-size %lu\n"
					"stderr-size %lu\n"
					"return-code %d\n"
					"status %s\n",
					j_res.count_inst, j_res.clk_thread, j_res.clk_ref_tsc,
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
	
	static int duck_packet_handle(DucknetIPv4Address src,
		uint16_t sport, char *content, int content_len) {
		// TODO: handle seq-num
		
		if (equals_to(content, content_len, "reboot")) {
			Utils::GG_reboot();
		} else {
			char *to_send = NULL;
			int to_send_len = 0;
			
			#define args content, content_len, to_send, to_send_len
			process_controls(args) || process_data(args) || process_judge(args);
			#undef args
			
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
