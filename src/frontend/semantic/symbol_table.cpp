#include "symbol_table.hpp"

#include <stdexcept>

namespace Spark::FrontEnd {

Symbol* SymbolTable::define(const Name* node, Symbol symbol) {
    if (hasSymbol(node)) {
        throw std::runtime_error("symbol already defined");
    }
    _symbols[node] = std::make_unique<Symbol>(std::move(symbol));
    return _symbols[node].get();
}

bool SymbolTable::hasSymbol(const Name* node) const noexcept {
    return _symbols.find(node) != _symbols.end();
}

Symbol* SymbolTable::symbolOf(const Name* node) {
    if (!hasSymbol(node)) {
        throw std::runtime_error("symbol not found");
    }
    return _symbols.at(node).get();
}

const Symbol* SymbolTable::symbolOf(const Name* node) const {
    if (!hasSymbol(node)) {
        throw std::runtime_error("symbol not found");
    }
    return _symbols.at(node).get();
}

} // Spark::FrontEnd
