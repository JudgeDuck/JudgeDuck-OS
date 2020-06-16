#include <inc/utils.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>
#include <inc/x86_64.hpp>

namespace Utils {
	void GG_reboot() {
		LDEBUG("GG, reboot");
		LINFO("GG, reboot");
		LWARN("GG, reboot");
		LERROR("GG, reboot");
		LFATAL("GG, reboot");
		Timer::microdelay((uint64_t) 3e6);  // 3s
		x86_64::reboot();
	}
}
