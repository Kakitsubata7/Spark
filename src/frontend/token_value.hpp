#pragma once

#include <string>

namespace Spark::FrontEnd {

/**
 * Token value type used by Flex and Bison.
 */
struct TokenValue {
    std::string lexeme;
    size_t line;
    size_t column;

    TokenValue() : line(0), column(0) { }
    TokenValue(std::string lexeme, size_t line, size_t column)
        : lexeme(std::move(lexeme)), line(line), column(column) { }
    TokenValue(std::string_view lexeme, size_t line, size_t column)
        : TokenValue(std::string(lexeme), line, column) { }
};

} // Spark::FrontEnd
