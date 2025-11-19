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

    LexerError() = default;
    LexerError(std::string message, size_t line, size_t column) noexcept
        : message(std::move(message)), line(line), column(column) { }

    bool operator==(const LexerError& rhs) const noexcept {
        return message == rhs.message && line == rhs.line && column == rhs.column;
    }

    bool operator!=(const LexerError& rhs) const noexcept {
        return *this == rhs;
    }
};

} // Spark::FrontEnd
