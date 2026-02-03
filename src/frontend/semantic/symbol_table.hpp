#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "frontend/ast/node.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

class SymbolTable {
private:
    std::vector<std::unique_ptr<Symbol>> _symbols;
    std::unordered_map<const Name*, Symbol*> _map;

public:
    SymbolTable() = default;

    Symbol* make(Symbol symbol);

    void set(const Name* node, Symbol* symbol);

    bool hasSymbol(const Name* node) const;

    Symbol* symbolOf(const Name* node);
    const Symbol* symbolOf(const Name* node) const;
};

} // Spark::FrontEnd
