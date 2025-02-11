#pragma once

#include <vector>

#include "lex_error.hpp"
#include "token.hpp"

namespace spark {

class LexResult {

    /* ===== Data ===== */

private:
    std::vector<Token> _tokens;
    std::vector<LexError> _errors;

public:
    const std::vector<Token>& tokens() const noexcept { return _tokens; }
    const std::vector<LexError>& errors() const noexcept { return _errors; }



    /* ===== Constructor ===== */

public:
    LexResult() = default;



    /* ===== Operations ===== */

    bool success() const noexcept { return _errors.empty(); }

    void pushToken(Token&& token) {
        _tokens.push_back(std::move(token));
    }

    void pushToken(const Token& token) {
        _tokens.push_back(token);
    }

    void pushError(LexError&& error) {
        _errors.push_back(std::move(error));
    }

    void pushError(const LexError& error) {
        _errors.push_back(error);
    }
};

} // spark
