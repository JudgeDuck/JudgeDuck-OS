#include <inc/scheduler.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>

namespace Scheduler {
	static enum {
		ACTIVE, IDLE
	} current_state;
	
	void init() {
		LDEBUG_ENTER_RET();
		
		current_state = ACTIVE;
	}
	
	void set_active() {
		current_state = ACTIVE;
	}
	
	static uint64_t last_active_timestamp_ns;
	
	// already X ms idle -> sleep Y ms
	const uint64_t idle_policy_ms[][2] = {
		{ 0, 0 },
		{ 1, 1 },
		{ 30, 2 },
	};
	const int n_idle_policies =
		sizeof(idle_policy_ms) / sizeof(idle_policy_ms[0]);
	
	void set_idle() {
		if (current_state == ACTIVE) {
			last_active_timestamp_ns = Timer::ns_since_epoch();
			current_state = IDLE;
		}
	}
	
	static uint64_t get_sleep_duration_ms() {
		if (current_state != IDLE) return 0;
		
		const uint64_t current_timestamp_ns = Timer::ns_since_epoch();
		const uint64_t idle_time_ms =
			(current_timestamp_ns - last_active_timestamp_ns) / 1000000;
		
		// pick the matched idle policy
		int i;
		for (i = n_idle_policies - 1; i > 0; i--) {
			if (idle_time_ms >= idle_policy_ms[i][0]) break;
		}
		
		return idle_policy_ms[i][1];
	}
	
	bool can_sleep() {
		return get_sleep_duration_ms() > 0;
	}
	
	void sleep() {
		uint64_t ms = get_sleep_duration_ms();
		
		if (ms > 0) {
			Timer::powersave_sleep(ms * 1000000);
		}
	}
}
