#include "options.hpp"
#include "logging/log.hpp"
#include <filesystem>
#include <ostream>
#include <fstream>
#include <exception>
#include <format>

namespace options {

    struct OutputDescriptor {
        std::filesystem::path path;
        logger::LogLevel minLevel;
        logger::LogLevel maxLevel;

        OutputDescriptor(std::filesystem::path path, logger::LogLevel minLevel, logger::LogLevel maxLevel) : 
            path{path},
            minLevel{minLevel},
            maxLevel{maxLevel}
            {}
    };

    SourceFile::SourceFile(std::filesystem::path path, std::ifstream &&stream) : 
        path{path},
        stream{std::move(stream)}
    {
        if (!this->stream.good()) {
            logger::log("Stream passed to SourceFile constructor is bad!");
        }
    }

    struct CurrentOptions {
        std::vector<std::filesystem::path> sourcepaths;
        std::vector<OutputDescriptor> outputpaths;
        std::filesystem::path targetpath;
        logger::LogLevel minLevel;
        logger::LogLevel maxLevel;

        CurrentOptions() : 
            sourcepaths{std::vector<std::filesystem::path>()}, 
            outputpaths{std::vector<OutputDescriptor>()}, 
            targetpath{std::filesystem::path()},
            minLevel{logger::LogLevel::DEBUG},
            maxLevel{logger::LogLevel::CRITICAL}
            {}
    };

    bool parseArgument(CurrentOptions &currentoptions, std::string &arg) {
        
        if (arg.length() == 0) {
            return false;
        }

        if (arg.starts_with("--")) {
            if (arg.starts_with("--target:")) {
                if (!currentoptions.targetpath.empty()) return false;
                currentoptions.targetpath = arg.substr(arg.find_first_of(':') + 1);
                return true;
            } else if (arg.starts_with("--source:")) {
                currentoptions.sourcepaths.emplace_back(arg.substr(arg.find_first_of(':') + 1));
                return true;
            } else if (arg.starts_with("--output:") || arg.starts_with("--dump:")) {
                currentoptions.outputpaths.emplace_back(arg.substr(arg.find_first_of(':') + 1), currentoptions.minLevel, currentoptions.maxLevel);
                return true;
            }
            return false;

        } else if (arg.starts_with("-")) {
            switch(arg[1]) {
                case 't':
                    if (!currentoptions.targetpath.empty()) return false;
                    currentoptions.targetpath = arg.substr(2);
                    return true;
                case 's':
                    currentoptions.sourcepaths.emplace_back(arg.substr(2));
                    return true;
                case 'o':
                case 'd':
                    currentoptions.outputpaths.emplace_back(arg.substr(2), currentoptions.minLevel, currentoptions.maxLevel);
                    return true;
                case 'l':
                    if (arg.length() != 3) return false;
                    int val = arg[2] - '0';
                    if (val < 0 || val > 4) return false;
                    currentoptions.minLevel = (logger::LogLevel) val;
                    return true;
            }
        }
        
        return false;
    }


    Options getCommandlineOptions(int count, char *args[]) {
        CurrentOptions options;
        
        bool allArgumentsAreGood = true;
        for (int i = 1; i < count; i++) {
            std::string argString = std::string(args[i]);
            if (!parseArgument(options, argString)) {
                allArgumentsAreGood = false;
                std::string message = std::format("Failed to parse argument {}: {}", i, argString);
                logger::log<logger::LogLevel::CRITICAL>(message);
            }
        }

        logger::addLogger(std::cout, options.minLevel, logger::LogLevel::WARN);
        logger::addLogger(std::cerr, logger::LogLevel::ERROR, logger::LogLevel::CRITICAL);

        for (OutputDescriptor output : options.outputpaths) {
            std::ofstream s(output.path);
            logger::addLogger(s, output.minLevel, output.maxLevel);
        }

        std::vector<SourceFile> sources = {};

        for (std::filesystem::path path : options.sourcepaths) {
            if (std::filesystem::exists(path)) {
                logger::log<logger::LogLevel::INFO>("Adding file \"", path.relative_path().string(), '"');
                std::ifstream stream = std::ifstream(path);
                if (stream.good()) {
                    sources.emplace_back(path, std::move(stream));
                } else {
                    logger::log<logger::LogLevel::ERROR>("Could not read file \"", path.relative_path().string(), '"');
                }
            } else {
                logger::log<logger::LogLevel::ERROR>("Could not find file at \"", path.relative_path().string(), '"');
            }
        }

        return allArgumentsAreGood ? Options(std::move(sources), (std::ostream *) nullptr) : Options();
    }

    Options::Options(std::vector<SourceFile> &&sources, std::ostream *target) : _sources{std::move(sources)}, _target{target} {}

    Options::Options() {
        _sources = std::vector<SourceFile>();
        _target = nullptr;
    }

    std::vector<SourceFile> &Options::sources() {
        return _sources;
    }

    const std::ostream *Options::target() {
        return _target;
    }
}