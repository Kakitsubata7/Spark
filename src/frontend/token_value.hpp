#pragma once

#include <string>

namespace Spark::FrontEnd {

/**
 * Represents a token value type used by Flex and Bison (as `yylval`).
 */
struct TokenValue {
    std::string lexeme;
    size_t line;
    size_t column;

    TokenValue() noexcept : line(0), column(0) { }
    TokenValue(std::string lexeme, size_t line, size_t column) noexcept
        : lexeme(std::move(lexeme)), line(line), column(column) { }

    bool operator==(const TokenValue& rhs) const noexcept {
        return lexeme == rhs.lexeme && line == rhs.line && column == rhs.column;
    }

    bool operator!=(const TokenValue& rhs) const noexcept {
        return !(*this == rhs);
    }
};

} // Spark::FrontEnd
