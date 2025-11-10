#pragma once

#include <vector>

#include "lexer_state.hpp"
#include "token.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a wrapper to Flex's lexer logics.
 */
class Lexer {
private:
    yyscan_t _scanner{};
    LexerState _lstate{};

public:
    explicit Lexer(std::istream& stream) noexcept;
    ~Lexer();

    [[nodiscard]]
    const std::vector<LexerError>& errors() const noexcept {
        return _lstate.errors;
    }

    Token lex() noexcept;
    std::vector<Token> lexAll() noexcept;

    void switchStream(std::istream& stream) noexcept {
        _lstate.stream = &stream;
    }
};

} // Spark::FrontEnd
