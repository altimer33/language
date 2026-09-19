
#include <iostream>
#include <exception>

#include "logging/log.hpp"
#include "utils/compiler-utils.hpp"
#include "precompilation/options.hpp"

int main(int argc, char* argv[]) {
    try {
    options::Options options = options::getCommandlineOptions(argc, argv);
    
    compiler::compile(options);
    } catch (std::runtime_error e) {
        logger::log<logger::LogLevel::ERROR>(e.what());
        return 1;
    }
    return 0;
}

