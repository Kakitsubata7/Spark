#pragma once

#include "token_type.hpp"
#include "token_value.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a token produced by a lexer, containing the type and the value of the token.
 */
struct Token {
    TokenType type;
    TokenValue value;

    Token(TokenType type, TokenValue value) noexcept : type(type), value(std::move(value)) { }

    bool operator==(const Token& rhs) const noexcept {
        return type == rhs.type && value == rhs.value;
    }

    bool operator!=(const Token& rhs) const noexcept {
        return !(*this == rhs);
    }
};

} // Spark::FrontEnd
