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
    Failable<T>::Failable(T value) :
        _valid{true},
        _value{value}
    {}

    template<typename T>
    Failable<T>::Failable(std::vector<CompileError*> &&errors) :
        _valid{false},
        _errors{errors},
        _refcount{new int(1)}
    {}

    template<typename T>
    Failable<T>::Failable(CompileError* error) :
        _valid{false},
        _errors{std::vector<CompileError*>{error}},
        _refcount{new int(1)}
    {}

    template<typename T>
    Failable<T>::Failable(Failable<T> &other) {
        // Intentional assignment and not comparison below
        if ((_valid = other._valid)) [[likely]] {
            _value = other._value;
        } else {
            _refcount = other._refcount;
            (*_refcount)++;
        }
    }

    template<typename T>
    Failable<T>::Failable(Failable<T> &&other) {
        // Intentional assignment and not comparison below
        if ((_valid = other._valid)) [[likely]] {
            _value = std::move(other._value);
        } else {
            _errors = std::move(other._errors);
            _refcount = other._refcount;
            other._refcount = nullptr;
        }
    }

    template<typename T>
    Failable<T> Failable<T>::operator= (Failable<T> &other) {
        if (this == &other) return;
        if (_valid) [[likely]] {
            _value.~vector();
        } else {
            _errors.~vector();
        }
        // Intentional assignment and not comparison below
        if ((_valid = other._valid)) [[likely]] {
            _value = other._value;
        } else {
            _errors = other._errors;
            _refcount = other._refcount;
            (*_refcount)++;
        }
    }

    template<typename T>
    Failable<T> Failable<T>::operator= (Failable<T> &&other) {
        if (this == &other) return;
        if (_valid) [[likely]] {
            _value.~vector();
        } else {
            _errors.~vector();
        }
        // Intentional assignment and not comparison below
        if ((_valid = other._valid)) [[likely]] {
            _value = std::move(other._value);
        } else {
            _errors = std::move(other._errors);
            _refcount = other._refcount;
            other._refcount = nullptr;
        }
    }

    template<typename T>
    Failable<T>::~Failable() {
        if (_valid) [[likely]] {
            _value.~T();
        } else {
            if (_refcount != nullptr) {
                (*_refcount)--;
                if (*_refcount == 0) {
                    for (CompileError *error : _errors) {
                        delete error;
                    }
                }
            }
            _errors.~vector();
        }
    }
}