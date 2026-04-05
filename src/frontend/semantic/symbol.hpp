#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include <unordered_map>

#include "frontend/ast/node.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

class SemanticType;

/**
 * Represents the declaration kind of a symbol.
 */
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
     * Type of the symbol.
     */
    SemanticType* type;

    /**
     * Name the symbol bound to.
     */
    std::string name;

    /**
     * Start location of the symbol's definition.
     */
    Location start;

    /**
     * End location of the symbol's definition.
     */
    Location end;
};

/**
 * Represents a data structure that manages `Symbol` instances.
 * Instances will be deallocated during the table's destruction.
 */
class SymbolTable {
private:
    std::vector<std::unique_ptr<Symbol>> _symbols;

public:
    Symbol* make(Symbol symbol) {
        return _symbols.emplace_back(std::make_unique<Symbol>(symbol)).get();
    }
};

/**
 * Represents a map that binds a `Node` instance to a `Symbol` instance.
 */
class NodeSymbolMap {
private:
    std::unordered_map<const Node*, Symbol*> _map;

public:
    [[nodiscard]]
    Symbol* get(const Node* node) const {
        auto it = _map.find(node);
        return it == _map.end() ? nullptr : it->second;
    }

    void set(const Node* node, Symbol* symbol) {
        assert(node != nullptr);

        if (symbol == nullptr) {
            _map.erase(node);
        } else {
            _map[node] = symbol;
        }
    }

    [[nodiscard]]
    bool has(const Node* node) const {
        return _map.find(node) != _map.end();
    }
};

} // Spark::FrontEnd
