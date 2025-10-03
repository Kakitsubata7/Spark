#pragma once

#include <cstddef>
#include <string>

namespace Spark::FrontEnd {

enum class TokenType {
    Keyword,
    Identifier,
    Discard,
    Operator,
    Delimiter
};

struct Token {
    TokenType type;
    std::string lexeme;
    size_t line;
    size_t column;
};

} // Spark::FrontEnd
