#ifndef DUCK_LOGGER_H
#define DUCK_LOGGER_H

#include <inc/vga_buffer.hpp>

#include <stdio.h>

namespace Logger {
	enum LogLevel {
		LL_FATAL,
		LL_ERROR,
		LL_WARN,
		LL_INFO,
		LL_DEBUG,
	};
	
	struct TimedLogger {
		TimedLogger(VGA_Buffer::ColorCode colorcode, char name, bool mute);
		TimedLogger(const TimedLogger &logger);
		~TimedLogger();
		
		const char name;
		const bool mute;
		VGA_Buffer::ColorCode saved_colorcode;
	};
	
	extern LogLevel log_level;
	
	void set_log_level(LogLevel level);
	
	void init();
	
	TimedLogger get_logger(VGA_Buffer::ColorCode colorcode,
		LogLevel level, char name);
	
	struct FunctionLoggerGuard {
		FunctionLoggerGuard(TimedLogger (*getlogger)(), char name,
			const char* func, bool log_enter, bool log_ret)
			: getlogger(getlogger), name(name), func(func), log_ret(log_ret) {
			if (log_enter) {
				auto logger = getlogger();
				if (!logger.mute) {
					printf("%s%s", func, log_ret ? " start" : "");
				}
			}
		}
		
		~FunctionLoggerGuard() {
			if (log_ret) {
				auto logger = getlogger();
				if (!logger.mute) {
					printf("%s done", func);
				}
			}
		}
		
		TimedLogger (*getlogger)();
		const char name;
		const char* const func;
		const bool log_ret;
	};
	
	TimedLogger LFATAL();
	TimedLogger LERROR();
	TimedLogger LWARN();
	TimedLogger LINFO();
	TimedLogger LDEBUG();

	void LFATAL(const char * fmt, ...);
	void LERROR(const char * fmt, ...);
	void LWARN(const char * fmt, ...);
	void LINFO(const char * fmt, ...);
	void LDEBUG(const char * fmt, ...);
}

using Logger::LFATAL;
using Logger::LERROR;
using Logger::LWARN;
using Logger::LINFO;
using Logger::LDEBUG;

#define LINFO_ENTER()      Logger::FunctionLoggerGuard __function_logger_guard(LINFO,  'I', __PRETTY_FUNCTION__, true,  false)
#define LINFO_RET()        Logger::FunctionLoggerGuard __function_logger_guard(LINFO,  'I', __PRETTY_FUNCTION__, false, true)
#define LINFO_ENTER_RET()  Logger::FunctionLoggerGuard __function_logger_guard(LINFO,  'I', __PRETTY_FUNCTION__, true,  true)
#define LDEBUG_ENTER()     Logger::FunctionLoggerGuard __function_logger_guard(LDEBUG, 'D', __PRETTY_FUNCTION__, true,  false)
#define LDEBUG_RET()       Logger::FunctionLoggerGuard __function_logger_guard(LDEBUG, 'D', __PRETTY_FUNCTION__, false, true)
#define LDEBUG_ENTER_RET() Logger::FunctionLoggerGuard __function_logger_guard(LDEBUG, 'D', __PRETTY_FUNCTION__, true,  true)

#endif // !DUCK_LOGGER_H
