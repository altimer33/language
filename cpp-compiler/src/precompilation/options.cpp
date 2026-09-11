#include "options.hpp"
#include "logging/log.hpp"

namespace options {
    Options getCommandlineOptions(int count, char *args[]) {
        for (int i = 1; i < count; i++) {
            logger::log<logger::LogLevel::INFO>("Argument ", i, ": ", args[i]);
        }
        return Options();
    }

    Options::Options(std::vector<std::istream*> &sources, std::ostream *target) {
        _sources = std::move(sources);
        _target = target;
    }

    Options::Options() {
        _sources = std::vector<std::istream*>();
        _target = nullptr;
    }
}