#include <chrono>
#include <format>
#include <string>

namespace logger {
    std::vector<Logger> loggers = {};

    Logger::Logger(std::ostream &outstream, const LogLevel minLevel, const LogLevel maxLevel) : 
        outstream{outstream}, minLevel{minLevel}, maxLevel{maxLevel} 
        { }

    void addLogger(std::ostream &outstream, const LogLevel minLevel, const LogLevel maxLevel) {
        loggers.emplace_back(outstream, minLevel, maxLevel);
    }

    consteval auto getStringValueOfLogLevel(const LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
        }
    }

    template<LogLevel level, typename... T>
    void Logger::log(T &&...values) const {
        if (level >= minLevel && level <= maxLevel) {
            (outstream << ... << std::forward<T>(values));
        }
    }

    auto getTimestampAsString() {
        return std::format("{:%H:%M:%S}", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()));
    }

    template<LogLevel level, typename... T>
    void log(T &&...values) {
        auto timestamp = getTimestampAsString();

        for (const Logger &l : loggers) {
            l.log<level>('[', timestamp, "] [", getStringValueOfLogLevel(level), "] ", std::forward<T>(values)..., '\n');
        }
    }
}