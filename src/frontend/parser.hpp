#pragma once

#include "ast/ast.hpp"
#include "parser_error.hpp"
#include "result.hpp"

namespace Spark::FrontEnd {

class Parser {
public:
    Parser() = default;

    Result<AST::AST, ParserError> parse(std::istream& stream);
};

} // Spark::FrontEnd
