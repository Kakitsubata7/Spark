#pragma once

#include "ast.hpp"
#include "parser/parser_context.hpp"

namespace Spark::FrontEnd {

class Parser {
public:
    Parser() = default;

    AST parse(std::istream& stream);
};

} // Spark::FrontEnd
