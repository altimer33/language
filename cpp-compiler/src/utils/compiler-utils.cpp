#include "compiler-utils.hpp"

namespace compiler {
    Location CompileError::location() {
        return _location;
    }

    std::string CompileError::message() {
        return _message;
    }
}