#pragma once

#include "ast/ast.hpp"
#include "parser_error.hpp"

namespace Spark::Compiler {

struct ParserContext {
    AST::AST& ast;
    ParserError error;

    explicit ParserContext(AST::AST& ast, ParserError error = {}) noexcept : ast(ast), error(std::move(error)) { }

    template <typename T, typename... Args>
    T* makeNode(Args&&... args) {
        return ast.make<T>(std::forward<Args>(args)...);
    }
};

} // Spark::Compiler
