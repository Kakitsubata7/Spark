#pragma once

#include "frontend/ast.hpp"
#include "parser_error.hpp"

namespace Spark::FrontEnd {

struct ParserContext {
    AST& ast;
    ParserError error;

    explicit ParserContext(AST& ast, ParserError error = {}) noexcept : ast(ast), error(std::move(error)) { }

    template <typename T, typename... Args>
    T* makeNode(Args&&... args) {
        return ast.make<T>(std::forward<Args>(args)...);
    }
};

} // Spark::FrontEnd
