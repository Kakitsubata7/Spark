#pragma once

#include <string>

namespace Spark::FrontEnd {

struct ParserError {
    std::string _message;
    size_t _line = 0;
    size_t _column = 0;

    ParserError() = default;
    ParserError(std::string message, size_t line, size_t column) noexcept
        : _message(std::move(message)), _line(line), _column(column) { }
};

} // Spark::FrontEnd
