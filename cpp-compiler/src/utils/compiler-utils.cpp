#include "compiler-utils.hpp"

#include "tokens/lexer.hpp"
#include <format>

namespace compiler {
    void compile(options::Options &opts) {
        std::vector<options::SourceFile> &sources = opts.sources();
        std::vector<lexer::Lexer> lexers = std::vector<lexer::Lexer>();
        lexers.reserve(sources.size());
        for (options::SourceFile &source : sources) {
            lexers.emplace_back(std::move(source));
        }
        for (lexer::Lexer &lexer : lexers) {
            logger::log("Tokenizing source \"", lexer.file().relative_path().string(), '"');
            while (!lexer.done()) {
                auto val = lexer.nextToken();
                if (val.valid()) {
                    logger::log("Token: ", val.value().string());
                } else {
                    for (CompileError *error : val.errors()) {
                        logger::log<logger::LogLevel::ERROR>(error->message());
                    }
                }
            }
            logger::log("Finished tokenizing source \"", lexer.file().relative_path().string(), '"');
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
}