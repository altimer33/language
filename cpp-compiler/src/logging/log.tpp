#include <vector>
#include <string>

namespace logger {
    extern std::vector<logger::Logger> loggers;

    consteval auto getStringValueOfLogLevel(const LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
        }
    }

    std::string getTimestampAsString();

    template<LogLevel level, typename... T>
    void Logger::log(T &&...values) const {
        if (level >= minLevel && level <= maxLevel) {
            (outstream << ... << std::forward<T>(values));
        }
    }

    template<LogLevel level, typename... T>
    void log(T &&...values) {
        std::string timestamp = getTimestampAsString();

        for (const Logger &l : loggers) {
            l.log<level>('[', timestamp, "] [", getStringValueOfLogLevel(level), "] ", std::forward<T>(values)..., '\n');
        }
    }
}