#include "lexer.hpp"

#include <new>

#include <lex.yy.hpp>

#include "semantic_type.hpp"

namespace Spark::FrontEnd {

Lexer::Lexer(std::istream& stream, std::optional<std::string_view> filename)
    : _filename(filename),
      _scanner(nullptr),
      _srcbuf(stream),
      _lstate(_srcbuf, 1, 1) {
    yylex_init_extra(&_lstate, &_scanner);
    if (_scanner == nullptr) {
        throw std::bad_alloc();
    }
    yyset_in(nullptr, _scanner);
}

Lexer::~Lexer() {
    if (_scanner != nullptr) {
        yylex_destroy(_scanner);
    }
}

Lexer::Lexer(Lexer&& other) noexcept
    : _filename(std::move(other._filename)),
      _scanner(other._scanner),
      _srcbuf(std::move(other._srcbuf)),
      _lstate(std::move(other._lstate)) {
    if (_scanner != nullptr) {
        yyset_extra(&_lstate, _scanner);
    }

    other._scanner = nullptr;
}

Lexer& Lexer::operator=(Lexer&& other) noexcept {
    if (this != &other) {
        _filename = std::move(other._filename);
        if (_scanner != nullptr) {
            yylex_destroy(_scanner);
        }
        _scanner = other._scanner;
        _srcbuf = std::move(other._srcbuf);
        _lstate = std::move(other._lstate);
        if (_scanner != nullptr) {
            yyset_extra(&_lstate, _scanner);
        }

        other._scanner = nullptr;
    }
    return *this;
}

Token Lexer::lex() {
    SemanticType s;
    yy::parser::location_type loc;
    TokenType type = static_cast<TokenType>(yylex(&s, &loc, _scanner));
    const TokenValue& value = s.as<TokenValue>();
    return Token{type, value.lexeme, value.start.lineno, value.start.columnno};
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

} // Spark::FrontEnd
