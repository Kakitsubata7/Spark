#pragma once

#include "token_type.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a token produced by a lexer, containing the type and the value of the token.
 */
struct Token {
    TokenType type;
    std::string lexeme;
    Location start;
    Location end;

    Token(TokenType type, std::string lexeme, Location start, Location end) noexcept
        : type(type), lexeme(std::move(lexeme)), start(start), end(end) { }
    Token() noexcept : Token(TokenType::EndOfFile, "", {0, 0}, {0, 0}) { }

    // TODO: Delete this constructor
    Token(TokenType type, std::string lexeme, size_t line, size_t column) noexcept
        : Token(type, std::move(lexeme), Location(line, column), Location(line, column)) { }

    bool operator==(const Token& rhs) const noexcept {
        return type == rhs.type && lexeme == rhs.lexeme /* TODO: && start == rhs.start && end == rhs.end */;
    }

    bool operator!=(const Token& rhs) const noexcept {
        return !(*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << "Token(" << token.type << ", "
                       << "\"" << token.lexeme << "\"" << ", "
                       << token.start.lineno << ':' << token.end.columnno << " - "
                       << token.end.lineno << ':' << token.end.columnno
           << ")";
        return os;
    }
};

} // Spark::FrontEnd
