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

    explicit ParserContext(AST& ast, std::vector<Error> errors = {}) noexcept
        : _ast(ast), _errors(std::move(errors)) { }

    template <typename T, typename... Args>
    T* makeNode(Args&&... args) {
        return _ast.make<T>(std::forward<Args>(args)...);
    }

    void addError(Error error) noexcept {
        _errors.push_back(std::move(error));
    }

    void addError(std::string message, Location start, Location end) noexcept {
        _errors.emplace_back(std::move(message), start, end);
    }
};

} // Spark::FrontEnd
