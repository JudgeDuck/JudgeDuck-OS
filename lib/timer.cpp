#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <inc/timer.hpp>
#include <inc/x86_64.hpp>
#include <inc/utils.hpp>
#include <inc/logger.hpp>
#include <inc/lapic.hpp>

namespace Timer {
	uint64_t tsc_freq, tsc_epoch;
	uint32_t ext_freq;
	
	static int get_tsc_clock_ratio(uint32_t *a, uint32_t *b) {
		uint32_t _a, _b, _c, _d;
		x86_64::cpuid(0x15, &_a, &_b, &_c, &_d);
		if (_b == 0) {
			return -1;
		} else {
			*a = _b;
			*b = _a;
			return 0;
		}
	}
	
	static uint32_t get_base_freq() {
		uint32_t _a, _b, _c, _d;
		x86_64::cpuid(0x16, &_a, &_b, &_c, &_d);
		return _a;
	}
	
	static void detect_cpu_speed_intel() {
		uint32_t base_freq = 0;
		uint32_t tsc_clock_ratio_a, tsc_clock_ratio_b;
		base_freq = get_base_freq();
		int r = get_tsc_clock_ratio(&tsc_clock_ratio_a, &tsc_clock_ratio_b);
		if (base_freq == 0 || r != 0) {
			// Maybe QEMU CPU ?
			// TODO: Detect older Intel CPUs
			// 2020-04-03: Running QEMU on i3-8100 (3.6 GHz)
			tsc_freq = (uint64_t) 3600 * 1000000;
			ext_freq = 1000 * 1000000;
		} else {
			tsc_freq = (uint64_t) base_freq * 1000000;
			ext_freq = base_freq * tsc_clock_ratio_b / tsc_clock_ratio_a * 1000000;
		}
	}
	
	static void detect_cpu_speed_others() {
		// TODO: Detect tsc freq from PIT
		unimplemented();
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
