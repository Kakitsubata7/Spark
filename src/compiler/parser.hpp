#pragma once

#include "ast.hpp"
#include "parser_error.hpp"
#include "result.hpp"

namespace Spark::Compiler {

class Parser {
public:
    Parser() = default;

    Result<AST::AST, ParserError> parse(std::istream& stream);
};

} // Spark::Compiler
