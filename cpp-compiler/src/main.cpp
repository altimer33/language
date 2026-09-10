
#include <iostream>

#include "logging/log.hpp"

int main(int argc, char* argv[]) {
    logger::addLogger(std::cout, logger::LogLevel::DEBUG, logger::LogLevel::CRITICAL);

    logger::log<logger::LogLevel::DEBUG>("Hello, World!");
}