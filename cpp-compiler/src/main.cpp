
#include <iostream>

#include "logging/log.hpp"

using namespace logger;

int main(int argc, char* argv[]) {
    addLogger(std::cout, LogLevel::DEBUG, LogLevel::CRITICAL);

    log<LogLevel::DEBUG>("Hello, World!");
}