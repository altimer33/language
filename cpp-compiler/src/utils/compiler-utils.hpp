#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include "precompilation/options.hpp"

namespace compiler {
    struct Location {
        size_t line;
        size_t column;
        const std::filesystem::path *file;

        Location(const size_t line, const size_t column, const std::filesystem::path *file);
    };

    class CompileError {
    private:
        Location _location;
        std::string _message;
    public:
        Location location();
        std::string message();
        std::string string();

        CompileError(Location location, std::string message);
    };

    template <typename T>
    struct Failable {
    private:
        bool _valid;
        union {
            struct {
                std::vector<CompileError*> _errors;
                int *_refcount;
            };
            T _value;
        };

    public:
        bool valid() const;

        const std::vector<CompileError*> &errors() const;

        T &value();

        Failable(T value);

        Failable(std::vector<CompileError*> &&errors);
        Failable(CompileError* error);

        Failable(Failable<T> &other);
        Failable(Failable<T> &&other);
        Failable<T> operator= (Failable<T> &other);
        Failable<T> operator= (Failable<T> &&other);
        ~Failable();
    };

    void compile(options::Options &opts);
}

#include "compiler-utils.tpp"