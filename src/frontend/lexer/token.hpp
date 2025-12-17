#pragma once

#include "token_type.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a token produced by a lexer, containing the type and the value of the token.
 */
struct Token {
    TokenType type;
    std::string lexeme;
    // TODO: Delete `line` and `column` fields
    size_t line = 0;
    size_t column = 0;
    Location start;
    Location end;

    // TODO: Delete this constructor
    Token(TokenType type, std::string lexeme, size_t line, size_t column) noexcept
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) { }

    Token(TokenType type, std::string lexeme, Location start, Location end) noexcept
        : type(type), lexeme(std::move(lexeme)), start(start), end(end) { }
    Token() noexcept : Token(TokenType::EndOfFile, "", {0, 0}, {0, 0}) { }

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
