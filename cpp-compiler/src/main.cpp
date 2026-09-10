
#include <iostream>

#include "logging/log.hpp"
#include "utils/compiler-utils.hpp"

int main(int argc, char* argv[]) {
    logger::addLogger(std::cout, logger::LogLevel::DEBUG, logger::LogLevel::WARN);
    logger::addLogger(std::cerr, logger::LogLevel::ERROR, logger::LogLevel::CRITICAL);

    logger::log<logger::LogLevel::DEBUG>("Hello, World!");
}