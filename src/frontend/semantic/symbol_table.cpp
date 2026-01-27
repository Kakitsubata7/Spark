#include "symbol_table.hpp"

#include <stdexcept>

namespace Spark::FrontEnd {

void SymbolTable::define(const Node* node, Symbol symbol) {
    _symbols[node] = std::make_unique<Symbol>(std::move(symbol));
}

bool SymbolTable::hasSymbol(const Node* node) const noexcept {
    return _symbols.find(node) != _symbols.end();
}

const Symbol& SymbolTable::symbolOf(const Node* node) const {
    if (!hasSymbol(node)) {
        throw std::runtime_error("symbol not found");
    }
    return *_symbols.at(node);
}

} // Spark::FrontEnd
