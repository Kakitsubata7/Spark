#pragma once

#include <ostream>
#include <string>

namespace spark {

enum class TokenType {
    Identifier,
    Keyword,
    Symbol,
    Integer,
    Real,
    Boolean,
    String,
    Nil,
    Ignore,
    Comment,
    End,
    Unknown
};

struct Token {
    const TokenType type;
    const std::string lexeme;

    Token(TokenType type, const char* lexeme) : type(type), lexeme(lexeme) { }
    Token(TokenType type, const std::string& lexeme) : type(type), lexeme(lexeme) { }

    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

} // spark
