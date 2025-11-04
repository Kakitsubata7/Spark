#pragma once

#include <istream>

#include "lexer_state.hpp"
#include "token.hpp"

namespace Spark::FrontEnd {

class Lexer {
private:
    yyscan_t _scanner{};
    LexerState _lstate{};

public:
    explicit Lexer(std::istream& stream);
    ~Lexer();

    Token lex();
};

} // Spark::FrontEnd
