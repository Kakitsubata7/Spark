#pragma once

#include <vector>

#include "frontend/lexer.hpp"

namespace Spark::FrontEnd {

class TokenProducer {
public:
    virtual ~TokenProducer() = default;

    virtual const Token& peek() = 0;
    virtual const Token& next() = 0;
};

class LexerTokenProducer final : public TokenProducer {
private:
    Lexer& _lexer;
    Token _token;
    bool _peeked = false;

public:
    explicit LexerTokenProducer(Lexer& lexer) noexcept : _lexer(lexer) { }

    const Token& peek() override;
    const Token& next() override;
};

class RewindLexerTokenProducer final : public TokenProducer {
private:
    Lexer& _lexer;
    std::vector<Token> _tokens;
    bool _peeked = false;
    size_t _index = 0;

public:
    explicit RewindLexerTokenProducer(Lexer& lexer) noexcept : _lexer(lexer) { }

    const Token& peek() override;
    const Token& next() override;

    void rewind();
};

} // Spark::FrontEnd
