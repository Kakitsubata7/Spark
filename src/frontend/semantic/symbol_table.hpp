#pragma once

#include <memory>
#include <unordered_map>

#include "frontend/ast/node.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

class SymbolTable {
private:
    std::unordered_map<const Node*, std::unique_ptr<Symbol>> _symbols;

public:
    SymbolTable() = default;

    void define(const Node* node, Symbol symbol);
    bool hasSymbol(const Node* node) const noexcept;
    const Symbol& symbolOf(const Node* node) const;
};

} // Spark::FrontEnd
