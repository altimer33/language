#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <concepts>
#include "precompilation/options.hpp"
#include "logging/log.hpp"

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

    using ErrorList = std::vector<std::unique_ptr<CompileError>>;

    template <typename T>
    class Failable {
    private:
        template <typename U>
        friend class Failable;

        bool _valid;
        union {
            std::shared_ptr<ErrorList> _errors;
            T _value;
        };

    public:
        bool valid() const;

        const std::shared_ptr<ErrorList> &errors() const;

        T &value();
        T &operator*();
        T *operator->();

        Failable(T value);

        Failable(std::shared_ptr<ErrorList> &&errors);
        Failable(CompileError &&error);

        template<typename U>
            requires (!std::same_as<T, U> && std::constructible_from<T, U>)
        explicit Failable(Failable<U> &other);

        template<typename U>
            requires (!std::same_as<T, U> && std::constructible_from<T, U>)
        explicit Failable(Failable<U> &&other);

        template<typename U>
            requires (!std::same_as<T, U> && !std::constructible_from<T, U>)
        explicit Failable(Failable<U> &other);

        template<typename U>
            requires (!std::same_as<T, U> && !std::constructible_from<T, U>)
        explicit Failable(Failable<U> &&other);

        Failable(Failable<T> &other);
        Failable(Failable<T> &&other);
        Failable<T> operator= (Failable<T> &other);
        Failable<T> operator= (Failable<T> &&other);
        ~Failable();
    };

    template <>
    class Failable<void> {
    private:
        template <typename U>
        friend class Failable;

        bool _valid;
        union {
            std::shared_ptr<ErrorList> _errors;
        };

    public:
        bool valid() const;

        const std::shared_ptr<ErrorList> &errors() const;

        Failable(void);

        Failable(std::shared_ptr<ErrorList> &&errors);
        Failable(CompileError &&error);

        template<typename U>
            requires (!std::same_as<void, U>)
        explicit Failable(Failable<U> &other);

        template<typename U>
            requires (!std::same_as<void, U>)
        explicit Failable(Failable<U> &&other);

        Failable(Failable<void> &other);
        Failable(Failable<void> &&other);
        Failable<void> operator= (Failable<void> &other);
        Failable<void> operator= (Failable<void> &&other);
        ~Failable();
    };

    void compile(options::Options &opts);

    template<typename T>
        requires logger::Stringable<T>
    std::string vecToString(std::vector<T> &vec);

    template<typename T>
        requires logger::Stringable<T>
    std::string pvecToString(std::vector<T *> &vec);

    template<typename T>
        requires logger::Stringable<T>
    std::string pvecToString(std::vector<std::unique_ptr<T>> &vec);

    template<typename T>
        requires logger::Stringable<T>
    std::string pvecToString(std::vector<std::shared_ptr<T>> &vec);
}

#include "compiler-utils.tpp"