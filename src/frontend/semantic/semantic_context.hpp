#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "frontend/ast/node.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

class SemanticContext {
private:
    std::vector<std::unique_ptr<Symbol>> _symbols;

    std::unordered_map<const Node*, const Symbol*> _nodeSymMap;

public:
    SemanticContext() = default;

    SemanticContext(const SemanticContext& other) = delete;
    SemanticContext& operator=(const SemanticContext& other) = delete;

    SemanticContext(SemanticContext&& other) noexcept = default;
    SemanticContext& operator=(SemanticContext&& other) noexcept = default;

    const Symbol* make(Symbol symbol);

    void setSymbol(const Node* node, const Symbol* symbol);

    [[nodiscard]]
    const Symbol* symbolOf(const Node* node) const;

    [[nodiscard]]
    bool hasSymbol(const Node* node) const;
};

} // Spark::FrontEnd
