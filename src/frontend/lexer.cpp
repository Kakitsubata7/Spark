#include "lexer.hpp"

#include <new>

#include <lex.yy.hpp>

#include "semantic_type.hpp"

namespace Spark::FrontEnd {

Lexer::Lexer(std::istream& stream, SourceBuffer& srcbuf)
    : _scanner(nullptr), _lstate(stream, srcbuf, 1, 1) {
    yylex_init(&_scanner);
    if (_scanner == nullptr) {
        throw std::bad_alloc();
    }
    yyset_extra(&_lstate, _scanner);
}

Lexer::Lexer(std::istream& stream) : Lexer(stream, NullSourceBuffer::instance()) { }

Lexer::~Lexer() {
    if (_scanner != nullptr) {
        yylex_destroy(_scanner);
    }
}

Lexer::Lexer(Lexer&& other) noexcept
    : _scanner(other._scanner), _lstate(std::move(other._lstate)) {
    other._scanner = nullptr;
}

Lexer& Lexer::operator=(Lexer&& other) noexcept {
    if (this != &other) {
        _scanner = other._scanner;
        other._scanner = nullptr;
        _lstate = std::move(other._lstate);
    }
    return *this;
}

Token Lexer::lex() {
    SemanticType s;
    TokenType type = static_cast<TokenType>(yylex(&s, _scanner));
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

std::pair<std::vector<Token>, std::vector<Error>> Lexer::lexAll(std::istream& stream) {
    Lexer lexer(stream);
    std::vector<Token> tokens = lexer.lexAll();
    return std::make_pair(tokens, lexer.errors());
}

} // Spark::FrontEnd
