#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <inc/timer.hpp>
#include <inc/x86_64.hpp>
#include <inc/utils.hpp>
#include <inc/logger.hpp>
#include <inc/lapic.hpp>
#include <inc/utils.hpp>

namespace Timer {
	uint64_t tsc_freq, tsc_epoch;
	uint32_t ext_freq;
	uint64_t clk_freq;
	
	char cpu_model_name[49];
	
	static inline void cpuid(uint32_t eax, uint32_t ecx, uint32_t* abcd) {
		asm volatile("cpuid"
			: "=a"(abcd[0]), "=b"(abcd[1]), "=c"(abcd[2]), "=d"(abcd[3])
			: "a"(eax), "c"(ecx));
	}
	
	static void detect_cpu_model_name() {
		uint32_t abcd[4];
		
		for (uint32_t i = 0x80000002; i <= 0x80000004; i++) {
			cpuid(i, 0, abcd);
			for (int j = 0; j < 4; j++) {
				cpu_model_name[(i - 0x80000002) * 16 + j * 4 + 0] = abcd[j] & 0xff;
				cpu_model_name[(i - 0x80000002) * 16 + j * 4 + 1] = (abcd[j] >> 8) & 0xff;
				cpu_model_name[(i - 0x80000002) * 16 + j * 4 + 2] = (abcd[j] >> 16) & 0xff;
				cpu_model_name[(i - 0x80000002) * 16 + j * 4 + 3] = (abcd[j] >> 24) & 0xff;
			}
		}
		cpu_model_name[48] = '\0';
		LINFO("CPU Model Name: %s", cpu_model_name);
	}
	
	// returns: -1 if error
	static int get_tsc_clock_ratio(uint32_t *a, uint32_t *b, uint32_t *ext_freq) {
		uint32_t _a, _b, _c, _d;
		x86_64::cpuid(0x15, &_a, &_b, &_c, &_d);
		if (_b == 0) {
			return -1;
		} else {
			*a = _b;
			*b = _a;
			*ext_freq = _c;
			return 0;
		}
		
		// EAX Bits 31 - 00: An unsigned integer which is the denominator of the TSC/”core crystal clock” ratio.
		// EBX Bits 31 - 00: An unsigned integer which is the numerator of the TSC/”core crystal clock” ratio.
		// ECX Bits 31 - 00: An unsigned integer which is the nominal frequency of the core crystal clock in Hz.
	}
	
	static void detect_cpu_speed_intel() {
		ext_freq = 0;
		tsc_freq = 0;
		clk_freq = 0;
		
		// Special cases
		if (strstr(cpu_model_name, "i7-4790") != NULL) {
			ext_freq = 100 * 1000000;
			tsc_freq = 3600ull * 1000000;
			clk_freq = 3800ull * 1000000;
		} else if (strstr(cpu_model_name, "N100") != NULL) {
			clk_freq = 3400ull * 1000000;
		} else if (strstr(cpu_model_name, "i7-13700K") != NULL) {
			clk_freq = 5300ull * 1000000;
		}
		
		if (!ext_freq) {
			uint32_t tsc_clock_ratio_a, tsc_clock_ratio_b;
			int r = get_tsc_clock_ratio(&tsc_clock_ratio_a, &tsc_clock_ratio_b, &ext_freq);
			if (r != 0) {
				// Maybe QEMU CPU ?
				// TODO: Detect older Intel CPUs
				// 2020-04-03: Running QEMU on i3-8100 (3.6 GHz)
				tsc_freq = (uint64_t) 3600 * 1000000;
				ext_freq = 1000 * 1000000;
			} else {
				if (ext_freq == 0) {
					LWARN("Unknown ext_freq, assume 24MHz");
					ext_freq = 24 * 1000000;
				}
				tsc_freq = (uint64_t) ext_freq * tsc_clock_ratio_a / tsc_clock_ratio_b;
			}
		}
		
		if (!clk_freq) {
			// TODO: Detect clk_thread freq
			LWARN("Assume clk_freq Hz = round(tsc_freq, 100M)");
			clk_freq = Utils::round_down(tsc_freq + 50 * 1000000, 100ull * 1000000);
		}
		
		LINFO("ext_freq: %.2f MHz; tsc_freq: %.2f MHz; clk_freq: %.2f MHz",
			(double) ext_freq / 1000000,
			(double) tsc_freq / 1000000,
			(double) clk_freq / 1000000);
	}
	
	static void detect_cpu_speed_others() {
		// TODO: Detect tsc freq from PIT
		// TODO AMD!
		tsc_freq = (uint64_t) 3700 * 1000000;
		ext_freq = 100 * 1000000;
		clk_freq = tsc_freq;
		LWARN("[FIXME] Unknown CPU: Assume AMD @ 3.7GHz");
	}
	
	static void enable_performance_counters() {
		x86_64::wrmsr(x86_64::FIXED_CTR_CTRL, 2 * 0x111);  // userspace counters
		x86_64::wrmsr(x86_64::PERF_GLOBAL_CTRL, 7ull << 32);
		
		LDEBUG("Userspace performance counters enabled");
	}
	
	void init() {
		LDEBUG_ENTER_RET();
		
		char brand[13] = { 0 };
		x86_64::cpuid(0, NULL, (uint32_t *) brand, (uint32_t *) brand + 2, (uint32_t *) brand + 1);
		LDEBUG("CPU brand string: [%s]", brand);
		
		detect_cpu_model_name();
		
		if (memcmp(brand, "GenuineIntel", 12) == 0) {
			detect_cpu_speed_intel();
		} else {
			detect_cpu_speed_others();
		}
		
		tsc_epoch = get_tsc();
		
		LDEBUG("tsc_freq = %lu, ext_freq = %u", tsc_freq, ext_freq);
		
		enable_performance_counters();
	}
	
	void powersave_sleep(uint64_t ns) {
		LAPIC::timer_single_shot_ns(ns);
		x86_64::sti();
		x86_64::hlt();
	}
	
	void reset_performance_counters() {
		x86_64::wrmsr(x86_64::INST_RETIRED_ANY, 0);
		x86_64::wrmsr(x86_64::CPU_CLK_UNHALTED_THREAD, 0);
		x86_64::wrmsr(x86_64::CPU_CLK_UNHALTED_REF_TSC, 0);
	}
	
	void read_performance_counters(uint64_t &inst,
		uint64_t &clk_thread, uint64_t &clk_ref_tsc) {
		inst = x86_64::rdmsr(x86_64::INST_RETIRED_ANY);
		clk_thread = x86_64::rdmsr(x86_64::CPU_CLK_UNHALTED_THREAD);
		clk_ref_tsc = x86_64::rdmsr(x86_64::CPU_CLK_UNHALTED_REF_TSC);
	}
}
