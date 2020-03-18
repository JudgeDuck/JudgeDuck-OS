#ifndef DUCK_LOGGER_H
#define DUCK_LOGGER_H

#include <inc/time.hpp>
using Time::timef;
#include <inc/x86_64.hpp>
using x86_64::rdtsc;

#include <iostream>
#include <iomanip>

namespace Logger {
    enum LogLevel {
        LL_FATAL,
        LL_ERROR,
        LL_WARN,
        LL_INFO,
        LL_DEBUG,
    };

    template <typename UnderlyingT>
    struct TimedLoggerBase {
        TimedLoggerBase(UnderlyingT& underlying, char name, bool mute = false)
            : underlying(underlying), name(name), mute(mute)
        {
            if (mute)
                return;
            double tm = timef();
            *this << '[' << std::fixed << std::setprecision(6);
            if (tm != -1)
                *this << tm;
            else
                *this << "tsc " << rdtsc();
            *this << ' ' << name << ']' << ' ';
        }
        ~TimedLoggerBase() {
            if (mute)
                return;
            *this << '\n';
            underlying.flush();
        }
        template <typename T>
        TimedLoggerBase& operator<<(const T& t) {
            if (!mute)
                underlying << t;
            return *this;
        }
        UnderlyingT& underlying;
        const char name;
        const bool mute;
    };
    typedef TimedLoggerBase<decltype(std::cout)> TimedLogger;

    void init();
    void set_log_level(LogLevel level);
    TimedLogger get_logger(LogLevel level, char name);

    struct FunctionLoggerGuard {
        FunctionLoggerGuard(LogLevel level, char name, const char* func,
                            bool log_enter, bool log_ret)
                : level(level), name(name), func(func), log_ret(log_ret)
        {
            if (log_enter)
                get_logger(level, name) << func << " start"; 
        }
        ~FunctionLoggerGuard() {
            get_logger(level, name) << func << " done"; 
        }
        const LogLevel level;
        const char name;
        const char* const func;
        const bool log_ret;
    };
}
#define LFATAL() Logger::get_logger(Logger::LL_FATAL, 'F')
#define LERROR() Logger::get_logger(Logger::LL_ERROR, 'E')
#define LWARN()  Logger::get_logger(Logger::LL_WARN,  'W')
#define LINFO()  Logger::get_logger(Logger::LL_INFO,  'I')
#define LDEBUG() Logger::get_logger(Logger::LL_DEBUG, 'D')

#define LINFO_ENTER()      Logger::FunctionLoggerGuard __function_logger_guard(Logger::LL_INFO,  'I', __PRETTY_FUNCTION__, true,  false)
#define LINFO_RET()        Logger::FunctionLoggerGuard __function_logger_guard(Logger::LL_INFO,  'I', __PRETTY_FUNCTION__, false, true)
#define LINFO_ENTER_RET()  Logger::FunctionLoggerGuard __function_logger_guard(Logger::LL_INFO,  'I', __PRETTY_FUNCTION__, true,  true)
#define LDEBUG_ENTER()     Logger::FunctionLoggerGuard __function_logger_guard(Logger::LL_DEBUG, 'D', __PRETTY_FUNCTION__, true,  false)
#define LDEBUG_RET()       Logger::FunctionLoggerGuard __function_logger_guard(Logger::LL_DEBUG, 'D', __PRETTY_FUNCTION__, false, true)
#define LDEBUG_ENTER_RET() Logger::FunctionLoggerGuard __function_logger_guard(Logger::LL_DEBUG, 'D', __PRETTY_FUNCTION__, true,  true)

#endif // !DUCK_LOGGER_H