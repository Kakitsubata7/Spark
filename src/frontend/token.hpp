#pragma once

#include <string>

namespace Spark::FrontEnd {

enum class TokenType {
    None,
    Keyword,
    Identifier,
    Discard,
    Number,
    String,
    Operator,
    Delimiter,
    Comment,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string lexeme;
    size_t line;
    size_t column;

    Token(TokenType type, std::string lexeme, size_t line, size_t column)
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) { }
    Token() : type(TokenType::None), line(0), column(0) { }

    constexpr bool operator==(const Token& rhs) const {
        return type == rhs.type && lexeme == rhs.lexeme && line == rhs.line && column == rhs.column;
    }
};

} // Spark::FrontEnd
