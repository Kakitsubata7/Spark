#pragma once

#include <cstddef>
#include <string>

namespace Spark::FrontEnd {

struct TokenValue {
    std::string lexeme;
    size_t line = 0;
    size_t column = 0;

    TokenValue() = default;
    TokenValue(std::string lexeme, size_t line, size_t column) noexcept
        : lexeme(std::move(lexeme)), line(line), column(column) { }

    TokenValue(const TokenValue& other) = default;
    TokenValue& operator=(const TokenValue& other) = default;

    TokenValue(TokenValue&& other) = default;
    TokenValue& operator=(TokenValue&& other) = default;

    bool operator==(const TokenValue& rhs) const noexcept {
        return lexeme == rhs.lexeme && line == rhs.line && column == rhs.column;
    }

    bool operator!=(const TokenValue& rhs) const noexcept {
        return !(*this == rhs);
    }
};

} // Spark::FrontEnd
