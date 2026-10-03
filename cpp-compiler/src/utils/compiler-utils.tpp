#include "logging/log.hpp"

#include <string>
#include <format>
#include <ranges>

namespace compiler {
    template<typename T>
    bool Failable<T>::valid() const {
        return _valid;
    }

    template<typename T>
    const std::shared_ptr<ErrorList> &Failable<T>::errors() const {
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
    T &Failable<T>::operator*() {
        return value();
    }

    template<typename T>
    T *Failable<T>::operator->() {
        return &value();
    }

    template<typename T>
    Failable<T>::Failable(T value) :
        _valid{true},
        _value{value}
    {}

    template<typename T>
    Failable<T>::Failable(std::shared_ptr<ErrorList> &&errors) :
        _valid{false},
        _errors{std::move(errors)}
    {}

    template<typename T>
    Failable<T>::Failable(CompileError &&error) :
        _valid{false},
        _errors{std::make_shared<ErrorList>(ErrorList())}
    {
        _errors->push_back(std::make_unique<CompileError>(error));
    }

    template<typename T>
    Failable<T>::Failable(Failable<T> &other) :
        _valid{other._valid} {
        if ((_valid)) [[likely]] {
            _value = other._value;
        } else {
            _errors = other._errors;
        }
    }

    template<typename T>
    Failable<T>::Failable(Failable<T> &&other) : 
        _valid{other._valid} {
        if (_valid) [[likely]] {
            _value = std::move(other._value);
        } else {
            _errors = std::move(other._errors);
        }
    }

    template<typename T>
    Failable<T> Failable<T>::operator= (Failable<T> &other) {
        if (this == &other) return other;
        if (_valid) [[likely]] {
            _value.~T();
        } else {
            _errors.~shared_ptr();
        }
        _valid = other._valid;
        if (_valid) [[likely]] {
            _value = other._value;
        } else {
            _errors = other._errors;
        }
        return *this;
    }

    template<typename T>
    Failable<T> Failable<T>::operator= (Failable<T> &&other) {
        if (this == &other) return other;
        if (_valid) [[likely]] {
            _value.~T();
        } else {
            _errors.~shared_ptr();
        }
        _valid = other._valid;
        if (_valid) [[likely]] {
            _value = std::move(other._value);
        } else {
            _errors = std::move(other._errors);
        }
        return *this;
    }

    template<typename T>
    Failable<T>::~Failable() {
        if (_valid) [[likely]] {
            _value.~T();
        } else {
            _errors.~shared_ptr();
        }
    }

    template<typename T>
    template<typename U>
        requires (!std::same_as<T, U> && std::constructible_from<T, U>)
    Failable<T>::Failable(Failable<U> &other) {
        if (other._valid) [[likely]] {
            _valid = true;
            _value = static_cast<T>(other._value);
        } else {
            _valid = false;
            _errors = other._errors;
        }
    }

    template<typename T>
    template<typename U>
        requires (!std::same_as<T, U> && !std::constructible_from<T, U>)
    Failable<T>::Failable(Failable<U> &other) {
        if (other._valid) [[unlikely]] {
            throw std::runtime_error("Cannot convert Failable<T> to Failable<U> because Failable<T> is valid");
        }
        _valid = false;
        _errors = std::move(other._errors);
    }

    template<typename T>
    template<typename U>
        requires (!std::same_as<T, U> && std::constructible_from<T, U>)
    Failable<T>::Failable(Failable<U> &&other) {
        if (other._valid) [[likely]] {
            _valid = true;
            _value = static_cast<T &&>(other._value);
        } else {
            _valid = false;
            _errors = std::move(other._errors);
        }
    }

    template<typename T>
    template<typename U>
        requires (!std::same_as<T, U> && !std::constructible_from<T, U>)
    Failable<T>::Failable(Failable<U> &&other) {
        if (other._valid) [[unlikely]] {
            throw std::runtime_error("Cannot convert Failable<T> to Failable<U> because Failable<T> is valid");
        }
        _valid = false;
        _errors = std::move(other._errors);
    }
    
    template<typename U>
        requires (!std::same_as<void, U>)
    Failable<void>::Failable(Failable<U> &other) : 
        _valid{other._valid}
    {
        if (!_valid) {
            _errors = other._errors;
        }
    }
    
    template<typename U>
        requires (!std::same_as<void, U>)
    Failable<void>::Failable(Failable<U> &&other) : 
        _valid{other._valid}
    {
        if (!_valid) {
            _errors = std::move(other._errors);
        }
    }

    template<typename T>
        requires logger::Stringable<T>
    std::string pvecToString(const std::vector<T *> &vec) {
        return vec | std::views::transform([](T *val){return val->string();}) | std::views::join_with(std::string_view(", ")) | std::ranges::to<std::string>();
    }

    template<typename T>
        requires logger::Stringable<T>
    std::string pvecToString(const std::vector<std::unique_ptr<T>> &vec) {
        return vec | std::views::transform([](std::unique_ptr<T> &val){return val->string();}) | std::views::join_with(std::string_view(", ")) | std::ranges::to<std::string>();
    }

    template<typename T>
        requires logger::Stringable<T>
    std::string pvecToString(const std::vector<std::shared_ptr<T>> &vec) {
        return vec | std::views::transform([](std::shared_ptr<T> &val){return val->string();}) | std::views::join_with(std::string_view(", ")) | std::ranges::to<std::string>();
    }

    template<typename T>
        requires logger::Stringable<T>
    std::string vecToString(const std::vector<T> &vec) {
        return vec | std::views::transform([](T val){return val.string();}) | std::views::join_with(std::string_view(", ")) | std::ranges::to<std::string>();
    }
}