#pragma once

#include <string>

namespace Spark::FrontEnd {

struct TokenValue {
    std::string lexeme;
    size_t line;
    size_t column;

    TokenValue() : line(0), column(0) { }
    TokenValue(std::string lexeme, size_t line, size_t column)
        : lexeme(std::move(lexeme)), line(line), column(column) { }
};

} // Spark::FrontEnd
