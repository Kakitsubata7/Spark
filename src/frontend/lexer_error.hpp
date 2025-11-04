#pragma once

namespace Spark::FrontEnd {

struct LexerError {
    std::string message;
    size_t line;
    size_t column;

    LexerError(std::string message, size_t line, size_t column)
        : message(std::move(message)), line(line), column(column) { }
};

} // Spark::FrontEnd
