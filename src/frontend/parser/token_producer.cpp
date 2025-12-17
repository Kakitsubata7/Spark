#include "token_producer.hpp"

namespace Spark::FrontEnd {

const Token& LexerTokenProducer::peek()  {
    if (!_peeked) {
        _token = _lexer.lex();
        _peeked = true;
    }
    return _token;
}

const Token& LexerTokenProducer::next() {
    if (_peeked) {
        _peeked = false;
        return _token;
    }
    _token = _lexer.lex();
    return _token;
}

const Token& RewindLexerTokenProducer::peek() {
    if (_index == _tokens.size()) {
        _tokens.push_back(_lexer.lex());
    }
    return _tokens[_index];
}

const Token& RewindLexerTokenProducer::next() {
    if (_index == _tokens.size()) {
        _tokens.push_back(_lexer.lex());
    }
    ++_index;
    return _tokens[_index - 1];
}

void RewindLexerTokenProducer::rewind() {
    _index = 0;
}

} // Spark::FrontEnd
