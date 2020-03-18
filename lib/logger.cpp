#include <inc/logger.hpp>

namespace Logger {
    static LogLevel log_level;

    void init() {
        set_log_level(LL_DEBUG);
        LINFO_RET();
    }
    void set_log_level(LogLevel level) {
        log_level = level;
    }
    TimedLogger get_logger(LogLevel level, char name) {
        return TimedLogger(std::cout, name, level > log_level);
    }
}
