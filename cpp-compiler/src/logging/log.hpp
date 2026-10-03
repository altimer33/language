#pragma once

#include <fstream>
#include <ostream>
#include <concepts>
#include <string>

namespace logger {
    template<typename T>
    concept DirectlyPrintable = requires (std::ostream &stream, T val) { {stream << val} -> std::same_as<std::ostream &>; };

    template<typename T>
    concept Stringable = requires (T val) { { val.string() } -> std::convertible_to<std::string>; };

    template<typename T>
    concept Printable = DirectlyPrintable<T> || Stringable<T>;

    template<Stringable T>
    std::ostream &operator<<(std::ostream &stream, T value);

    enum class LogLevel : unsigned char {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        CRITICAL
    };
    
    template<LogLevel level = LogLevel::DEBUG, Printable... T>
    void log(T &&...values);

    class Logger {
    private:
        std::ostream &outstream;
        const LogLevel minLevel;
        const LogLevel maxLevel;
        bool isFileOutput;

        template<LogLevel level, Printable... T>
        void log(T &&...values) const;

        template<LogLevel level, Printable... T>
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