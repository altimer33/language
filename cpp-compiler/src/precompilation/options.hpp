#pragma once

#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

namespace options {
    struct SourceFile {
        std::filesystem::path path;
        std::ifstream &stream;

        SourceFile(std::filesystem::path path, std::ifstream &&stream);
    };

    class Options {
        std::vector<SourceFile> _sources;
        std::ostream *_target;

        Options(std::vector<SourceFile> &&sources, std::ostream *target);
        Options();

        friend Options getCommandlineOptions(int count, char* args[]);
    };

    Options getCommandlineOptions(int count, char* args[]);
}