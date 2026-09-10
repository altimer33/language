#pragma once

#include <iostream>
#include <filesystem>
#include <string>

namespace compiler {
    struct Location {
        size_t line;
        size_t column;
        std::istream* stream;
        std::filesystem::path* file;
    };

    class CompileError {
    private:
        Location _location;
        std::string _message;
    public:
        Location location();
        std::string message();
    };

    template <typename T>
    struct Failable {
    private:
        union {
            std::vector<CompileError*> _errors;
            T _value;
        };
        bool _valid;

    public:
        bool valid() const;

        const std::vector<CompileError*> &errors() const;

        T &value();

        Failable(T value);

        Failable(std::vector<CompileError*> &&errors);

        Failable(Failable<T> &other);
        Failable(Failable<T> &&other);
        Failable<T> operator= (Failable<T> &other);
        Failable<T> operator= (Failable<T> &&other);
        ~Failable();
    };
}

#include "compiler-utils.tpp"