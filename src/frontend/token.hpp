#pragma once

#include "token_type.hpp"
#include "token_value.hpp"

namespace Spark::FrontEnd {

struct Token {
    TokenType type;
    TokenValue value;

    Token(TokenType type, TokenValue value) : type(type), value(std::move(value)) { }
};

} // Spark::FrontEnd
