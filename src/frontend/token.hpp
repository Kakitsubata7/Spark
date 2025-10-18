#pragma once

#include <string>

namespace Spark::FrontEnd {

enum class TokenType {
    None,
    Keyword,
    Identifier,
    Discard,
    Operator,
    Delimiter,
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
};

} // Spark::FrontEnd
