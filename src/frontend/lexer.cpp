#include "lexer.hpp"

#include <lex.yy.hpp>

#include "semantic_type.hpp"

namespace Spark::FrontEnd {

Lexer::Lexer(std::istream& stream) : _lstate(1, 1, &stream) {
    yylex_init(&_scanner);
    yyset_extra(&_lstate, _scanner);
}

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
    TokenValue& value = s.as<TokenValue>();
    return Token{ type, value.lexeme, value.line, value.column };
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

Result<std::vector<Token>, std::vector<LexerError>> Lexer::lexAll(std::istream& stream) {
    Lexer lexer(stream);
    std::vector<Token> tokens = lexer.lexAll();

    if (lexer.hasError()) {
        return Result<std::vector<Token>, std::vector<LexerError>>::err(std::move(lexer._lstate.errors));
    }
    return Result<std::vector<Token>, std::vector<LexerError>>::ok(std::move(tokens));
}

} // Spark::FrontEnd
