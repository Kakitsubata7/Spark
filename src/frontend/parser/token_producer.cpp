#include "token_producer.hpp"

namespace Spark::FrontEnd {

Token lexDropComments(Lexer& lexer) {
    while (true) {
        Token token = lexer.lex();
        if (token.type == TokenType::LineComment || token.type == TokenType::BlockComment) {
            continue;
        }
        return token;
    }
}

const Token& SimpleTokenProducer::peek()  {
    if (!_peeked) {
        _token = lexDropComments(_lexer);
        _peeked = true;
    }
    return _token;
}

const Token& SimpleTokenProducer::next() {
    if (_peeked) {
        _peeked = false;
        return _token;
    }
    _token = lexDropComments(_lexer);
    return _token;
}

const Token& RewindTokenProducer::peek() {
    if (_index == _tokens.size()) {
        _tokens.push_back(lexDropComments(_lexer));
    }
    return _tokens[_index];
}

const Token& RewindTokenProducer::next() {
    if (_index == _tokens.size()) {
        _tokens.push_back(lexDropComments(_lexer));
    }
    ++_index;
    return _tokens[_index - 1];
}

void RewindTokenProducer::rewind() {
    _index = 0;
}

} // Spark::FrontEnd
