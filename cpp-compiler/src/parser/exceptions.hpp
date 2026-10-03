#pragma once

#include "utils/compiler-utils.hpp"
#include "tokens/lexer.hpp"

namespace compiler {
    class UnexpectedTokenError : public CompileError {
    public:
        UnexpectedTokenError(lexer::Token token);
    };
}