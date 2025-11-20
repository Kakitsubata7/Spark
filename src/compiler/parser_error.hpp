#pragma once

#include <sstream>
#include <string>

namespace Spark::Compiler {

struct ParserError {
    std::string message;
    size_t line = 0;
    size_t column = 0;

    ParserError() = default;
    ParserError(std::string message, size_t line, size_t column) noexcept
        : message(std::move(message)), line(line), column(column) { }

    bool operator==(const ParserError& rhs) const noexcept {
        return message == rhs.message && line == rhs.line && column == rhs.column;
    }

    bool operator!=(const ParserError& rhs) const noexcept {
        return *this == rhs;
    }
};

} // Spark::Compiler
