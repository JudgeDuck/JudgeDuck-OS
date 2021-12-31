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
	
	static uint32_t microcode_orig_rev = 0;
	
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
		} else if (equals_to(content, len, "cpu-temp")) {
			const uint32_t MSR_TEMPERATURE_TARGET = 0x1a2;
			const uint32_t IA32_THERM_STATUS = 0x19c;
			
			using x86_64::rdmsr;
			using Utils::extract_bits;
			uint64_t temp_target = extract_bits(rdmsr(MSR_TEMPERATURE_TARGET), 23, 16);
			uint64_t temp = temp_target - extract_bits(rdmsr(IA32_THERM_STATUS), 22, 16);
			
			res = content;
			sprintf(res, "cpu-temp %lu", temp);
		} else if (equals_to(content, len, "sysinfo")) {
			uint32_t rev = x86_64::get_microcode_revision();
			res = content;
			sprintf(res,
				"microcode-orig-rev = 0x%02x\n"
				"microcode-current-rev = 0x%02x\n",
				microcode_orig_rev, rev
			);
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
		
		if (16 == sscanf(content, "judge %lu %lu %lu "  // seq, tlns, mhlkb
			"%lu %lu %lu %lu %lu %lu %lu %lu"  // ELF, I, O, E
			"%lu %lu %lu %lu %lu",  // IB (off/len), OB (off/len/need_clear)
			&req.seq_num, &req.time_limit_ns, &req.memory_hard_limit_kb,
			&req.ELF.off, &req.ELF.len,
			&req.stdin.off, &req.stdin.len,
			&req.stdout.off, &req.stdout.len,
			&req.stderr.off, &req.stderr.len,
			&req.IB.off, &req.IB.len,
			&req.OB.off, &req.OB.len, &req.OB_need_clear)) {
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
				
				// Caclucate CPU clock MHz
				double clock_MHz = 0;
				uint64_t tsc_MHz = Timer::tsc_freq / 1000000;
				uint64_t ext_MHz = Timer::ext_freq / 1000000;
				if (j_res.clk_ref_tsc != 0) {
					clock_MHz = j_res.clk_thread / (double) j_res.clk_ref_tsc * tsc_MHz;
				}
				
				res_len = snprintf(res_str, sizeof(res_str),
					"ok-judge\n"
					"count-inst %lu\n"
					"clk-thread %lu\n"
					"clk-ref-tsc %lu\n"
					"clock-MHz %.5lf\n"
					"tsc-MHz %lu\n"
					"ext-MHz %lu\n"
					"time-ns %lu\n"
					"time-ns-ref-tsc %lu\n"
					"time-ns-real %lu\n"
					"time-tsc %lu\n"
					"memory-kb %lu\n"
					"memory-kb-accessed %lu\n"
					"stdout-size %lu\n"
					"stderr-size %lu\n"
					"return-code %d\n"
					"trap-num %d\n"
					"trap-epc 0x%lx\n"
					"trap-cr2 0x%lx\n"
					"status %s\n",
					j_res.count_inst, j_res.clk_thread, j_res.clk_ref_tsc,
					clock_MHz, tsc_MHz, ext_MHz,
					j_res.time_ns, j_res.time_ns_ref_tsc, j_res.time_ns_real, j_res.time_tsc,
					j_res.memory_kb,
					j_res.memory_kb_accessed,
					j_res.stdout_size, j_res.stderr_size, j_res.return_code,
					(int32_t) (uint32_t) j_res.trap_num,
					j_res.trap_epc,
					j_res.trap_cr2,
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
	
	static bool process_cache(char *content, int len, char *&res, int &res_len) {
		res = NULL;
		res_len = -1;
		content[len] = 0;
		
		uint64_t q_off, q_len;
		static char q_cache_name[2048];
		
		if (len > 64 && 3 == sscanf(content, "load-cache %s %lu %lu", q_cache_name, &q_off, &q_len)) {
			res = content;
			if (Judger::load_cache(q_cache_name, q_off, q_len, content + len - 64)) {
				res_len = sprintf(res, "ok-load-cache %s %lu %lu", q_cache_name, q_off, q_len);
			} else {
				res_len = sprintf(res, "fail-load-cache %s %lu %lu", q_cache_name, q_off, q_len);
			}
		} else if (len > 64 && 3 == sscanf(content, "store-cache %s %lu %lu", q_cache_name, &q_off, &q_len)) {
			res = content;
			if (Judger::store_cache(q_cache_name, q_off, q_len, content + len - 64)) {
				res_len = sprintf(res, "ok-store-cache %s %lu %lu", q_cache_name, q_off, q_len);
			} else {
				res_len = sprintf(res, "fail-store-cache %s %lu %lu", q_cache_name, q_off, q_len);
			}
		} else if (1 == sscanf(content, "info-cache %s", q_cache_name)) {
			static char tmp[2048];
			Judger::get_cache_info(q_cache_name, tmp);
			res = content;
			res_len = sprintf(res, "info-cache %s %s", q_cache_name, tmp);
		} else {
			return false;
		}
		
		if (res && res_len == -1) {
			res_len = strlen(res);
		}
		return true;
	}
	
	static bool process_special(char *content, int len, char *&res, int &res_len) {
		res = NULL;
		res_len = -1;
		content[len] = 0;
		
		if (equals_to(content, len, "query-all")) {
			// query-all:
			//   uptime
			//   statistics
			//   info-cache elf
			//   info-cache data
			//   cpu-temp
			
			res = content;
			res_len = 0;
			
			static char tmp_query[2048];
			char *tmp_res;
			int tmp_res_len;
			
			#define QUERY(s) tmp_query, sprintf(tmp_query, s), tmp_res, tmp_res_len
			#define APPEND_RESULT() { memcpy(res + res_len, tmp_res, tmp_res_len); res[(res_len += tmp_res_len)++] = '\n'; }
			
			process_controls(QUERY("uptime"));
			APPEND_RESULT();
			
			process_controls(QUERY("statistics"));
			APPEND_RESULT();
			
			process_cache(QUERY("info-cache elf"));
			APPEND_RESULT();
			
			process_cache(QUERY("info-cache data"));
			APPEND_RESULT();
			
			process_controls(QUERY("cpu-temp"));
			APPEND_RESULT();
			
			#undef QUERY
			#undef APPEND_RESULT
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
			process_controls(args) || 
			process_data(args) || 
			process_judge(args) ||
			process_cache(args) ||
			process_special(args);
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
	
	static void init_microcode() {
		LDEBUG_ENTER_RET();
		
		microcode_orig_rev = x86_64::get_microcode_revision();
		// TODO: update microcode?
	}
	
	static void enable_turbo_boost() {	
		LDEBUG_ENTER_RET();
		
		const uint32_t IA32_MISC_ENABLE = 416;
		const uint64_t TURBO_MODE_DISABLE = 1ul << 38;
		uint64_t misc = x86_64::rdmsr(IA32_MISC_ENABLE);
		x86_64::wrmsr(IA32_MISC_ENABLE, misc & ~TURBO_MODE_DISABLE);
		misc = x86_64::rdmsr(IA32_MISC_ENABLE) & TURBO_MODE_DISABLE;
		LINFO("Enabled turbo boost: %d", misc ? 0 : 1);
		const uint32_t IA32_PERF_CTL = 409;
		const uint32_t freq = 46;  // freq * 100
		x86_64::wrmsr(IA32_PERF_CTL, freq << 8);
		LINFO("CPU Freq set to %u MHz", freq * 100);
		// TODO: set to correct freq
	}
	
	void init() {
		LDEBUG_ENTER_RET();
		
		init_microcode();
		enable_turbo_boost();
		
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
