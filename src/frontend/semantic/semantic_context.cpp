#include "semantic_context.hpp"

namespace Spark::FrontEnd {

Symbol* SemanticContext::make(Symbol symbol) {
    return _symbols.emplace_back(std::make_unique<Symbol>(std::move(symbol))).get(); // TODO: Remove `std::move`
}

void SemanticContext::setSymbol(const Node* node, Symbol* symbol) {
    if (node == nullptr || symbol == nullptr) {
        return;
    }
    _nodeSymMap[node] = symbol;
}

Symbol* SemanticContext::symbolOf(const Node* node) const {
    if (node == nullptr) {
        return nullptr;
    }

    const auto it = _nodeSymMap.find(node);
    return it == _nodeSymMap.end() ? nullptr : it->second;
}

bool SemanticContext::hasSymbol(const Node* node) const {
    if (node == nullptr) {
        return false;
    }
    return _nodeSymMap.find(node) != _nodeSymMap.end();
}

} // Spark::FrontEnd
