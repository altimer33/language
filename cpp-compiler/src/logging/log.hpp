#pragma once

#include <ostream>

namespace logger {
    enum class LogLevel : unsigned char {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        CRITICAL
    };
    
    template<LogLevel level, typename... T>
    void log(T &&...values);

    class Logger {
    private:
        std::ostream &outstream;
        const LogLevel minLevel;
        const LogLevel maxLevel;

        template<LogLevel level, typename... T>
        void log(T &&...values) const;

        template<LogLevel level, typename... T>
        friend void log(T &&...values);
        
    public:
        Logger(std::ostream &outstream, const LogLevel minLevel, const LogLevel maxLevel);
    };

    void addLogger(std::ostream &outstream, const LogLevel minLevel, const LogLevel maxLevel);
}

#include "log.tpp"