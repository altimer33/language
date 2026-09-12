#include "log.hpp"

#include <format>
#include <chrono>
#include <string>

namespace logger {
    std::vector<logger::Logger> loggers = {};

    Logger::Logger(std::ostream &outstream, const LogLevel minLevel, const LogLevel maxLevel) : 
        outstream{outstream}, minLevel{minLevel}, maxLevel{maxLevel}, isFileOutput{false}
        { }

    void addLogger(std::ostream &outstream, const LogLevel minLevel, const LogLevel maxLevel) {
        loggers.emplace_back(outstream, minLevel, maxLevel);
    }

    Logger::Logger(std::ofstream &outstream, const LogLevel minLevel, const LogLevel maxLevel) : 
        outstream{outstream}, minLevel{minLevel}, maxLevel{maxLevel}, isFileOutput{true} 
        { }

    void addLogger(std::ofstream &outstream, const LogLevel minLevel, const LogLevel maxLevel) {
        loggers.emplace_back(outstream, minLevel, maxLevel);
    }

    std::string getTimestampAsString() {
        return std::format("{:%H:%M:%S}", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()));
    }

    void Logger::close() {
        if (isFileOutput) ((std::ofstream &) outstream).close();
    }

    void closeLoggers() {
        for(Logger &logger : loggers) {
            logger.close();
        }
    }
}