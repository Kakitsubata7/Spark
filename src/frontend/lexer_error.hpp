#pragma once

#include <string>

namespace Spark::FrontEnd {

/**
 * Represents an error occurred during lexing, containing the error message and the position the error occurred.
 */
struct LexerError {
    std::string message;
    size_t line;
    size_t column;

    LexerError(std::string message, size_t line, size_t column)
        : message(std::move(message)), line(line), column(column) { }
};

} // Spark::FrontEnd
