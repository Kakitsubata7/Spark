#pragma once

#include <optional>
#include <istream>
#include <vector>

#include "../result.hpp"
#include "token.hpp"

namespace Spark::FrontEnd {

class Lexer {
private:
    std::istream& _stream;

    size_t _line;
    size_t _column;

public:
    Lexer(std::istream& stream) : _stream(stream) { }

    Result<Token> lex() const;

    Result<std::vector<Token>> lexAll() const;
};

} // Spark::FrontEnd
