#include "lexer.hpp"

#include <new>

#include <lex.yy.hpp>

namespace Spark::FrontEnd {

Lexer::Lexer(SourceBuffer srcbuf)
    : _scanner(nullptr), _lstate(_srcbuf, 1, 1), _srcbuf(std::move(srcbuf)) {
    yylex_init_extra(&_lstate, &_scanner);
    if (_scanner == nullptr) {
        throw std::bad_alloc();
    }
    yyset_in(nullptr, _scanner);
}

Lexer::Lexer(std::istream& stream)
    : Lexer(SourceBuffer(stream)) { }

Lexer::~Lexer() {
    if (_scanner != nullptr) {
        yylex_destroy(_scanner);
    }
}

Token Lexer::lex() {
    yy::parser::semantic_type semanticType;
    yy::parser::location_type loc;
    TokenType type = static_cast<TokenType>(yylex(&semanticType, &loc, _scanner));
    const TokenValue& value = semanticType.as<TokenValue>();
    return Token{type, value.lexeme, value.start.line, value.start.column};
}

std::vector<Token> Lexer::lexAll() {
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

LexResult Lexer::lexAll(std::istream& stream) {
    Lexer lexer(stream);
    std::vector<Token> tokens = lexer.lexAll();
    return LexResult{std::move(tokens), std::move(lexer._srcbuf), std::move(lexer._diagnostics)};
}

} // Spark::FrontEnd
