#pragma once

#include "token_type.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a token produced by a lexer, containing the type and the value of the token.
 */
struct Token {
    TokenType type;
    std::string lexeme;
    // TODO: Use locations instead
    size_t line;
    size_t column;

    Token(TokenType type, std::string lexeme, size_t line, size_t column) noexcept
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) { }

    bool operator==(const Token& rhs) const noexcept {
        return type == rhs.type && lexeme == rhs.lexeme && line == rhs.line && column == rhs.column;
    }

    bool operator!=(const Token& rhs) const noexcept {
        return !(*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << "Token(" << token.type << ", "
                       << "\"" << token.lexeme << "\"" << ", "
                       << token.line << ":" << token.column
           << ")";
        return os;
    }
};

} // Spark::FrontEnd
