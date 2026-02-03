#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "frontend/ast/node.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

class SymbolTable {
private:
    std::unordered_map<const Name*, std::unique_ptr<Symbol>> _symbols;

public:
    SymbolTable() = default;

    template <typename... Args>
    void define(const Name* node, Args&&... args) {
        define(node, std::forward<Args>(args)...);
    }

    void define(const Name* node, Symbol symbol);

    bool hasSymbol(const Name* node) const noexcept;
    
    const Symbol& symbolOf(const Name* node) const;
};

} // Spark::FrontEnd
