#pragma once

#include <string>
#include <vector>

#include "lexer_error.hpp"

namespace Spark::FrontEnd {

/**
 * Represents additional information used by Flex (as `yyextra`).
 */
struct LexerState {
    size_t line;
    size_t column;
    std::istream* stream;
    std::string tokenBuffer;
    std::vector<LexerError> errors;
};

} // Spark::FrontEnd
