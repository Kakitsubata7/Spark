#pragma once

#include <vector>

#include "lexer_state.hpp"
#include "token.hpp"

namespace Spark::FrontEnd {

class Lexer {
private:
    yyscan_t _scanner{};
    LexerState _lstate{};

public:
    explicit Lexer(std::istream& stream) noexcept;
    ~Lexer();

    Token lex() noexcept;
    std::vector<Token> lexAll() noexcept;
};

} // Spark::FrontEnd
