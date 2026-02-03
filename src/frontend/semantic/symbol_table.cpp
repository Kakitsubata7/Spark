#include "symbol_table.hpp"

namespace Spark::FrontEnd {

Symbol* SymbolTable::make(Symbol symbol) {
    return _symbols.emplace_back(std::make_unique<Symbol>(std::move(symbol))).get();
}

void SymbolTable::set(const Name* node, Symbol* symbol) {
    _map[node] = symbol;
}

bool SymbolTable::hasSymbol(const Name* node) const {
    return _map.find(node) != _map.end();
}

Symbol* SymbolTable::symbolOf(const Name* node) {
    return _map.at(node);
}

const Symbol* SymbolTable::symbolOf(const Name* node) const {
    return _map.at(node);
}

} // Spark::FrontEnd
