#pragma once

#include <vector>

#include "frontend/ast.hpp"
#include "utils/error.hpp"

namespace Spark::FrontEnd {

class ParserContext {
private:
    AST& _ast;
    std::vector<Error> _errors;

public:
    [[nodiscard]]
    constexpr AST& ast() const noexcept { return _ast; }

    [[nodiscard]]
    constexpr const std::vector<Error>& errors() const noexcept { return _errors; }

    explicit ParserContext(AST& ast) noexcept : _ast(ast) { }

    void addError(Error error) noexcept {
        _errors.push_back(std::move(error));
    }

    void addError(std::string message, Location start, Location end) noexcept {
        _errors.emplace_back(std::move(message), start, end);
    }
};

} // Spark::FrontEnd
