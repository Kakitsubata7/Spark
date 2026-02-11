#include "semantic_context.hpp"

namespace Spark::FrontEnd {

const Symbol* SemanticContext::make(Symbol symbol) {
    return _symbols.emplace_back(std::make_unique<Symbol>(std::move(symbol))).get();
}

void SemanticContext::setSymbol(const Node* node, const Symbol* symbol) {
    if (node == nullptr || symbol == nullptr) {
        return;
    }
    _nodeSymMap[node] = symbol;
}

const Symbol* SemanticContext::symbolOf(const Node* node) const {
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
