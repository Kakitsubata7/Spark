#pragma once

#include <cstddef>
#include <string>

#include "utils/location.hpp"

namespace Spark::FrontEnd {

struct TokenValue {
    std::string lexeme;
    Location start;
    Location end;

    TokenValue() = default;
    TokenValue(std::string lexeme, Location start, Location end) noexcept
        : lexeme(std::move(lexeme)), start(start), end(end) { }

    TokenValue(const TokenValue& other) = default;
    TokenValue& operator=(const TokenValue& other) = default;

    TokenValue(TokenValue&& other) = default;
    TokenValue& operator=(TokenValue&& other) = default;

    bool operator==(const TokenValue& rhs) const noexcept {
        return lexeme == rhs.lexeme && start == rhs.start && end == rhs.end;
    }

    bool operator!=(const TokenValue& rhs) const noexcept {
        return !(*this == rhs);
    }
};

} // Spark::FrontEnd
