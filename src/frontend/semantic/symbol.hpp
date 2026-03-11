#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include <unordered_map>

#include "frontend/ast/node.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

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
     * Declaration `Name` node.
     */
    Name* node;

    /**
     * Retrieves the name string of the symbol.
     * @return Name string of the symbol.
     */
    [[nodiscard]]
    std::string_view name() const noexcept {
        return node->value.str();
    }

    /**
     * Retrieves the start location of the symbol name.
     * @return Start location of the symbol name.
     */
    [[nodiscard]]
    Location start() const noexcept {
        return node->start;
    }

    /**
     * Retrieves the end location of the symbol name.
     * @return End location of the symbol name.
     */
    [[nodiscard]]
    Location end() const noexcept {
        return node->end;
    }
};

class SymbolTable {
private:
    std::vector<std::unique_ptr<Symbol>> _symbols;

public:
    Symbol* make(Symbol symbol) {
        return _symbols.emplace_back(std::make_unique<Symbol>(symbol)).get();
    }
};

class NodeSymbolMap {
private:
    std::unordered_map<const Node*, Symbol*> _map;

public:
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
};

} // Spark::FrontEnd
