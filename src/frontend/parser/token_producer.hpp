#pragma once

#include <vector>

#include "frontend/lexer.hpp"

namespace Spark::FrontEnd {

/**
 * Represents an abstract token producer for parsers.
 * A token producer should drop comment tokens (`TokenType::LineComment` and `TokenType::BlockComment`).
 */
class TokenProducer {
protected:
    Lexer& _lexer;

public:
    explicit TokenProducer(Lexer& lexer) noexcept : _lexer(lexer) { }
    virtual ~TokenProducer() = default;

    virtual const Token& peek() = 0;
    virtual const Token& next() = 0;

    friend class RewindTokenProducer;
};

/**
 * Represents a concrete `TokenProducer` implementation that uses a `Lexer` reference and only keep one `Token`
 * instance alive at once.
 */
class SimpleTokenProducer final : public TokenProducer {
private:
    Token _token;
    bool _peeked = false;

public:
    explicit SimpleTokenProducer(Lexer& lexer) noexcept : TokenProducer(lexer) { }

    const Token& peek() override;
    const Token& next() override;
};

/**
 * Represents a concrete `TokenProducer` implementation that uses a `Lexer` reference but keeps all `Token` instances
 * alive and supports rewinding to first token.
 */
class RewindTokenProducer final : public TokenProducer {
private:
    std::vector<Token> _tokens;
    bool _peeked = false;
    size_t _index = 0;

public:
    explicit RewindTokenProducer(Lexer& lexer) noexcept : TokenProducer(lexer) { }
    explicit RewindTokenProducer(const TokenProducer& producer) noexcept : TokenProducer(producer._lexer) { }

    const Token& peek() override;
    const Token& next() override;

    void rewind();
};

} // Spark::FrontEnd
