#ifndef DUCK_SCHEDULER_H
#define DUCK_SCHEDULER_H

namespace Scheduler {
	void init();
	
	void set_active();
	void set_idle();
	bool can_sleep();
	void sleep();
}

#endif
