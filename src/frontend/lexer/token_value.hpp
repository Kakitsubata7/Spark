#pragma once

#include <cstddef>
#include <string>

namespace Spark::FrontEnd {

struct TokenValue {
    std::string lexeme;
    size_t lineno = 0;
    size_t columnno = 0;

    TokenValue() = default;
    TokenValue(std::string lexeme, size_t lineno, size_t columnno) noexcept
        : lexeme(std::move(lexeme)), lineno(lineno), columnno(columnno) { }

    TokenValue(const TokenValue& other) = default;
    TokenValue& operator=(const TokenValue& other) = default;

    TokenValue(TokenValue&& other) = default;
    TokenValue& operator=(TokenValue&& other) = default;

    bool operator==(const TokenValue& rhs) const noexcept {
        return lexeme == rhs.lexeme && lineno == rhs.lineno && columnno == rhs.columnno;
    }

    bool operator!=(const TokenValue& rhs) const noexcept {
        return !(*this == rhs);
    }
};

} // Spark::FrontEnd
