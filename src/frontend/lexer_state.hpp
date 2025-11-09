#pragma once

#include <string>
#include <vector>

#include "lexer_error.hpp"

namespace Spark::FrontEnd {

/**
 * Represents additional information used by Flex (as `yyextra`).
 */
struct LexerState {
    /**
     * Current line number of the state.
     */
    size_t line;

    /**
     * Current column number of the state.
     */
    size_t column;

    /**
     * Pointer to the `std::istream` being processed by the state.
     */
    std::istream* stream;

    /**
     * The opening/closing delimiter used by the currently processing string literal.
     */
    char stringDelimiter;

    /**
     * String buffer to store a currently processing token.
     */
    std::string tokenBuffer;

    /**
     * Starting line number of the currently processing token.
     */
    size_t tokenLine;

    /**
     * Starting column number of the currently processing token.
     */
    size_t tokenColumn;

    /**
     * Lexing errors encountered by the state.
     */
    std::vector<LexerError> errors;
};

} // Spark::FrontEnd
