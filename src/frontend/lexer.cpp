#include "lexer.hpp"

#include <cstring>

using namespace Spark;
using namespace Spark::FrontEnd;

Result<Token> Lexer::lex() const {
    // Skip space characters
    char current;
    while (true) {
        if (!_stream.get(current)) {
            // TODO: return error
        }
        if (!std::isspace(current)) {
            break;
        }
    }

    // Check for comment syntax
    char next;
    if (_stream.get(next)) {
        _stream.unget().unget();
        if (current == '/' && (next == '/' || next == '*')) {
            return lexComment(_stream);
        }
    }

    // Check for other token types
    _stream.unget();
    if (std::isalpha(current) || current == '_') {
        return lexWord(_stream);
    }
    if (std::isdigit(current)) {
        return lexNumber(_stream);
    }
    if (current == '"' || current == '\'') {
        return lexString(_stream);
    }
    if (std::ispunct(current)) {
        return lexPunctuator(_stream);
    }

    // TODO: return error;
}

Result<std::vector<Token>> Lexer::lexAll() const {
    std::vector<Token> tokens;
    while (_stream) {
        Result<Token> r = lex();
        if (r.hasError()) {
            // TODO: return error;
        }
        tokens.push_back(r.value());
    }
    // TODO: return tokens;
}
