#pragma once

#include <memory>
#include <vector>

#include "frontend/ast/node.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

enum class SymbolKind {
    Var, Func, Type, Module
};

struct Symbol {
    /**
     * Declaration kind of the symbol.
     */
    SymbolKind kind;

    /**
     * Whether the symbol is reassignable or not.
     */
    bool isReassignable;

    /**
     * Declaration `Name` node.
     */
    Name* name;
};

class SymbolTable {
private:
    std::vector<std::unique_ptr<Symbol>> _symbols;

public:
    Symbol* make(Symbol symbol) {
        return _symbols.emplace_back(std::make_unique<Symbol>(symbol)).get();
    }
};

} // Spark::FrontEnd
