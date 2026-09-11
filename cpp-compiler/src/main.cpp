
#include <iostream>

#include "logging/log.hpp"
#include "utils/compiler-utils.hpp"
#include "precompilation/options.hpp"

int main(int argc, char* argv[]) {
    logger::addLogger(std::cout, logger::LogLevel::DEBUG, logger::LogLevel::WARN);
    logger::addLogger(std::cerr, logger::LogLevel::ERROR, logger::LogLevel::CRITICAL);

    options::getCommandlineOptions(argc, argv);
}