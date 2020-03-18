#ifndef DUCK_LOGGER_H
#define DUCK_LOGGER_H

#include <inc/timer.hpp>
using Timer::secf_since_epoch;
#include <inc/x86_64.hpp>
using x86_64::rdtsc;
#include <inc/vga_buffer.hpp>

#include <iostream>
#include <iomanip>
#include <functional>

namespace Logger {
    enum LogLevel {
        LL_FATAL,
        LL_ERROR,
        LL_WARN,
        LL_INFO,
        LL_DEBUG,
    };

    struct TimedLogger {
        TimedLogger(VGA_Buffer::ColorCode colorcode, char name, bool mute)
            : name(name), mute(mute), saved_colorcode(VGA_Buffer::writer->color_code)
        {
            if (mute) return;

            VGA_Buffer::writer->color_code = colorcode;

            double tm = secf_since_epoch();
            *this << '[' << std::fixed << std::setprecision(6);
            if (tm != -1) *this << tm;
            else *this << "tsc " << rdtsc();
            *this << ' ' << name << ']' << ' ';
        }
        ~TimedLogger() {
            if (mute) return;
            std::cout.flush();
            VGA_Buffer::writer->color_code = saved_colorcode;
            std::cout << std::endl;
        }
        template <typename T>
        TimedLogger& operator<<(const T& t) {
            if (!mute) std::cout << t;
            return *this;
        }
        const char name;
        const bool mute;
        const VGA_Buffer::ColorCode saved_colorcode;
    };

    extern LogLevel log_level;

    static inline void set_log_level(LogLevel level) {
        log_level = level;
    }
    static inline void init() {
        set_log_level(LL_DEBUG);
        // LINFO_RET();
    }
    static inline TimedLogger get_logger(VGA_Buffer::ColorCode colorcode,
                                         LogLevel level, char name) {
        return TimedLogger(colorcode, name, level > log_level);
    }

    struct FunctionLoggerGuard {
        FunctionLoggerGuard(std::function<TimedLogger()> getlogger, char name,
                            const char* func, bool log_enter, bool log_ret)
            : getlogger(getlogger), name(name), func(func), log_ret(log_ret)
        {
            if (log_enter) getlogger() << func << (log_ret ? " start" : "");
        }
        ~FunctionLoggerGuard() {
            if (log_ret) getlogger() << func << " done"; 
        }
        const std::function<TimedLogger()> getlogger;
        const char name;
        const char* const func;
        const bool log_ret;
    };
}

static inline Logger::TimedLogger LFATAL() {
    return Logger::get_logger(
                    VGA_Buffer::ColorCode::generate(VGA_Buffer::Color::Red,
                                                    VGA_Buffer::Color::White),
                    Logger::LL_FATAL, 'F');
}
static inline Logger::TimedLogger LERROR() {
    return Logger::get_logger(
                    VGA_Buffer::ColorCode::generate(VGA_Buffer::Color::Red,
                                                    VGA_Buffer::Color::Black),
                    Logger::LL_ERROR, 'E');
}
static inline Logger::TimedLogger LWARN() {
    return Logger::get_logger(
                    VGA_Buffer::ColorCode::generate(VGA_Buffer::Color::Yellow,
                                                    VGA_Buffer::Color::Black),
                    Logger::LL_WARN, 'W');
}
static inline Logger::TimedLogger LINFO() {
    return Logger::get_logger(
                    VGA_Buffer::ColorCode::generate(VGA_Buffer::Color::White,
                                                    VGA_Buffer::Color::Black),
                    Logger::LL_INFO, 'I');
}
static inline Logger::TimedLogger LDEBUG() {
    return Logger::get_logger(
                    VGA_Buffer::ColorCode::generate(VGA_Buffer::Color::LightGray,
                                                    VGA_Buffer::Color::Black),
                    Logger::LL_DEBUG, 'D');
}

#define LINFO_ENTER()      Logger::FunctionLoggerGuard __function_logger_guard(LINFO,  'I', __PRETTY_FUNCTION__, true,  false)
#define LINFO_RET()        Logger::FunctionLoggerGuard __function_logger_guard(LINFO,  'I', __PRETTY_FUNCTION__, false, true)
#define LINFO_ENTER_RET()  Logger::FunctionLoggerGuard __function_logger_guard(LINFO,  'I', __PRETTY_FUNCTION__, true,  true)
#define LDEBUG_ENTER()     Logger::FunctionLoggerGuard __function_logger_guard(LDEBUG, 'D', __PRETTY_FUNCTION__, true,  false)
#define LDEBUG_RET()       Logger::FunctionLoggerGuard __function_logger_guard(LDEBUG, 'D', __PRETTY_FUNCTION__, false, true)
#define LDEBUG_ENTER_RET() Logger::FunctionLoggerGuard __function_logger_guard(LDEBUG, 'D', __PRETTY_FUNCTION__, true,  true)

#endif // !DUCK_LOGGER_H