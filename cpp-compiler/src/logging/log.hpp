#pragma once

#include <fstream>
#include <ostream>

namespace logger {
    enum class LogLevel : unsigned char {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        CRITICAL
    };
    
    template<LogLevel level = LogLevel::DEBUG, typename... T>
    void log(T &&...values);

    class Logger {
    private:
        std::ostream &outstream;
        const LogLevel minLevel;
        const LogLevel maxLevel;
        bool isFileOutput;

        template<LogLevel level, typename... T>
        void log(T &&...values) const;

        template<LogLevel level, typename... T>
        friend void log(T &&...values);
        
    public:
        Logger(std::ostream &outstream, const LogLevel minLevel, const LogLevel maxLevel);
        Logger(std::ofstream &outstream, const LogLevel minLevel, const LogLevel maxLevel);
        
        void close();
    };

    void addLogger(std::ostream &outstream, const LogLevel minLevel, const LogLevel maxLevel);
    void addLogger(std::ofstream &outstream, const LogLevel minLevel, const LogLevel maxLevel);

    void closeLoggers();
}

#include "log.tpp"