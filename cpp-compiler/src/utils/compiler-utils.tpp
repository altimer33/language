#include "logging/log.hpp"

namespace compiler {
    template<typename T>
    bool Failable<T>::valid() const {
        return _valid;
    }

    template<typename T>
    const std::vector<CompileError*> &Failable<T>::errors() const {
        if (_valid) {
            logger::log<logger::LogLevel::CRITICAL>("Attempted to get errors of a Failable object when the result was valid.");
            throw std::runtime_error("Attempted to get errors of a Failable object when the result was valid.");
        } else [[likely]] {
            return _errors;
        }
    }

    template<typename T>
    T &Failable<T>::value() {
        if (!_valid) {
            logger::log<logger::LogLevel::CRITICAL>("Attempted to access an invalid result from Failable object.");
            throw std::runtime_error("Attempted to access an invalid result from Failable object.");
        } else [[likely]] {
            return _value;
        }
    }

    template<typename T>
    Failable<T>::Failable(T value) : _valid{true}, _value{value} {}

    template<typename T>
    Failable<T>::Failable(std::vector<CompileError*> &&errors) : _valid{false}, _errors{errors} {}

    template<typename T>
    Failable<T>::Failable(Failable<T> &other) {
        // Intentional assignment and not comparison below
        if ((_valid = other._valid)) {
            _errors = other._errors;
        } else {
            _value = other._value;
        }
    }

    template<typename T>
    Failable<T>::Failable(Failable<T> &&other) {
        // Intentional assignment and not comparison below
        if ((_valid = other._valid)) {
            _errors = std::move(other._errors);
        } else {
            _value = std::move(other._value);
        }
    }

    template<typename T>
    Failable<T> Failable<T>::operator= (Failable<T> &other) {
        if (this == &other) return;
        if (_valid) {
            _errors.~vector();
        } else {
            _value.~T();
        }
        // Intentional assignment and not comparison below
        if ((_valid = other._valid)) {
            _errors = other._errors;
        } else {
            _value = other._value;
        }
    }

    template<typename T>
    Failable<T> Failable<T>::operator= (Failable<T> &&other) {
        if (this == &other) return;
        if (_valid) {
            _errors.~vector();
        } else {
            _value.~T();
        }
        // Intentional assignment and not comparison below
        if ((_valid = other._valid)) {
            _errors = std::move(other._errors);
        } else {
            _value = std::move(other._value);
        }
    }

    template<typename T>
    Failable<T>::~Failable() {
        if (_valid) {
            _errors.~vector();
        } else {
            _value.~T();
        }
    }
}