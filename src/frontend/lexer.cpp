#include "lexer.hpp"

#include <lex.yy.hpp>

namespace Spark::FrontEnd {

Lexer::Lexer(std::istream& stream) noexcept {
    yylex_init(&_scanner);
    _lstate.line = 1;
    _lstate.column = 1;
    _lstate.stream = &stream;
    yyset_extra(&_lstate, _scanner);
}

Lexer::~Lexer() {
    yylex_destroy(_scanner);
}

Token Lexer::lex() noexcept {
    TokenValue value;
    TokenType type = static_cast<TokenType>(yylex(&value, _scanner));
    return {type, value};
}

std::vector<Token> Lexer::lexAll() noexcept {
    std::vector<Token> tokens;
    while (true) {
        Token token = lex();
        if (token.type == TokenType::EndOfFile) {
            break;
        }
        tokens.push_back(token);
    }
    return tokens;
}

} // Spark::FrontEnd
