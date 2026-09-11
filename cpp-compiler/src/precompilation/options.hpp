#pragma once

#include <iostream>
#include <vector>

namespace options {
    class Options {
        std::vector<std::istream*> _sources;
        std::ostream *_target;

        Options(std::vector<std::istream*> &sources, std::ostream *target);
        Options();

        friend Options getCommandlineOptions(int count, char* args[]);
    };

    Options getCommandlineOptions(int count, char* args[]);
}