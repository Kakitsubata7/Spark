#include "lexer.hpp"

#include <lex.yy.hpp>

namespace Spark::FrontEnd {

Lexer::Lexer(std::istream& stream) {
    yylex_init(&_scanner);
    _lstate.line = 1;
    _lstate.column = 1;
    yyset_extra(&_lstate, _scanner);
}

Lexer::~Lexer() {
    yylex_destroy(&_scanner);
}

Token Lexer::lex() {
    TokenValue value;
    TokenType type = static_cast<TokenType>(yylex(&value, _scanner));
    return {type, value};
}

} // Spark::FrontEnd
