#pragma once

#include "ast.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

struct ParseResult {
    AST ast;
    Diagnostics diagnostics;

    ParseResult() noexcept = default;
    ParseResult(AST ast, Diagnostics diagnostics) noexcept
        : ast{std::move(ast)}, diagnostics{std::move(diagnostics)} {}
};

class Parser {
public:
    Parser() = default;

    static ParseResult parse(std::istream& stream);
};

} // Spark::FrontEnd
