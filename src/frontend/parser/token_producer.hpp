#pragma once

#include "frontend/lexer.hpp"

namespace Spark::FrontEnd {

class TokenProducer {
public:
    virtual ~TokenProducer() = default;

    virtual Token& next() = 0;
};

class LexerTokenProducer final : public TokenProducer {
private:
    Lexer& _lexer;
    Token _token;

public:
    explicit LexerTokenProducer(Lexer& lexer) noexcept : _lexer(lexer) { }

    Token& next() override {
        return _token = std::move(_lexer.lex());
    }
};

class BufferedLexerTokenProducer final : public TokenProducer {
private:
    Lexer& _lexer;
    std::vector<Token> _tokens;

public:
    explicit BufferedLexerTokenProducer(Lexer& lexer) noexcept : _lexer(lexer) { }

    Token& next() override {
        return _tokens.emplace_back(std::move(_lexer.lex()));
    }
};

} // Spark::FrontEnd
