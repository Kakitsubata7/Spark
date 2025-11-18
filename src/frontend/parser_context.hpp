#pragma once

#include "ast/ast.hpp"
#include "parser_error.hpp"

namespace Spark::FrontEnd {

struct ParserContext {
    AST::AST& ast;
    ParserError error;

    explicit ParserContext(AST::AST& ast, ParserError error = {}) noexcept : ast(ast), error(std::move(error)) { }
};

} // Spark::FrontEnd
