#include "compiler-utils.hpp"

#include "tokens/lexer.hpp"
#include "parser/declarations.hpp"

#include <format>
#include <ranges>

namespace compiler {
    void compile(options::Options &opts) {
        std::vector<options::SourceFile> &sources = opts.sources();
        std::vector<lexer::Lexer> lexers = std::vector<lexer::Lexer>();
        std::vector<parser::Parser> parsers = std::vector<parser::Parser>();
        lexers.reserve(sources.size());
        parsers.reserve(sources.size());
        for (options::SourceFile &source : sources) {
            lexers.emplace_back(std::move(source));
            parsers.emplace_back(lexers[lexers.size() - 1]);
        }
        for (int i = 0; i < sources.size(); i++) {
            lexer::Lexer &l = lexers[i];
            parser::Parser &p = parsers[i];
            logger::log("Parsing source: ", l.file().relative_path().string());
            Failable<void> result = p.parseFile();
            if (result.valid()) {
                logger::log(p.string());
            } else {
                for (std::unique_ptr<CompileError> &error : *result.errors()) {
                    logger::log<logger::LogLevel::ERROR>(error->string());
                }
            }
        }
    }

    Location::Location(const size_t line, const size_t column, const std::filesystem::path *file) : 
        line{line},
        column{column},
        file{file}
    {}

    Location CompileError::location() {
        return _location;
    }

    std::string CompileError::message() {
        return _message;
    }

    std::string CompileError::string() {
        return std::format("{} (at {}:{} in {})", _message, _location.line, _location.column, _location.file->relative_path().string());
    }

    CompileError::CompileError(Location location, std::string message) :
        _location{location},
        _message{message}
    {}

    bool Failable<void>::valid() const {
        return _valid;
    }

    const std::shared_ptr<ErrorList> &Failable<void>::errors() const {
        if (_valid) {
            logger::log<logger::LogLevel::CRITICAL>("Attempted to get errors of a Failable object when the result was valid.");
            throw std::runtime_error("Attempted to get errors of a Failable object when the result was valid.");
        } else [[likely]] {
            return _errors;
        }
    }

    Failable<void>::Failable() :
        _valid{true}
    {}

    Failable<void>::Failable(std::shared_ptr<ErrorList> &&errors) :
        _valid{false},
        _errors{errors}
    {}

    Failable<void>::Failable(CompileError &&error) :
        _valid{false},
        _errors{std::make_shared<ErrorList>(ErrorList())}
    {
        _errors->push_back(std::make_unique<CompileError>(error));
    }

    Failable<void>::Failable(Failable<void> &other) :
        _valid{other._valid}
    {
        if (!_valid) {
            _errors = other._errors;
        }
    }

    Failable<void>::Failable(Failable<void> &&other) :
        _valid{other._valid}
    {
        if (!_valid) {
            _errors = std::move(other._errors);
        }
    }

    Failable<void> Failable<void>::operator= (Failable<void> &other) {
        if (this == &other) return other;
        if (_valid) [[unlikely]] {
            _errors.~shared_ptr();
        }
        _valid = other._valid;
        // Intentional assignment and not comparison below
        if ((_valid = other._valid)) [[likely]] {
            _errors = other._errors;
        }
        return *this;
    }

    Failable<void> Failable<void>::operator= (Failable<void> &&other) {
        if (this == &other) return other;
        if (_valid) [[unlikely]] {
            _errors.~shared_ptr();
        }
        _valid = other._valid;
        if (!_valid) [[unlikely]] {
            _errors = std::move(other._errors);
        }
        return *this;
    }

    Failable<void>::~Failable() {
        if (_valid) [[unlikely]] {
            _errors.~shared_ptr();
        }
    }
}