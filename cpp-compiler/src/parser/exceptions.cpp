#include "exceptions.hpp"

#include <format>

#include "utils/compiler-utils.hpp"
#include "tokens/lexer.hpp"

namespace compiler {
    UnexpectedTokenError::UnexpectedTokenError(lexer::Token token) : 
        CompileError(token.location(), std::format("Unexpected token \"{}\"", *token.sourceString()))
    {}
}