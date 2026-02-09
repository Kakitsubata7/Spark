#pragma once

#include "frontend/ast/node.hpp"

namespace Spark::FrontEnd {

struct Symbol {
    /**
     * Pointer to the definition `Name` node.
     */
    Name* node;

    /**
     * Whether the symbol is reassignable or not.
     */
    bool isReassignable;

    /**
     * Whether the symbol is a reference or not.
     */
    bool isReference;

    /**
     * Retrieves the interned name of the symbol.
     * @return Interned name of the symbol.
     */
    [[nodiscard]]
    InternedNameValue name() const noexcept {
        return node->value;
    }

    /**
     * Retrieves the start location of the definition.
     * @return Start location of the definition.
     */
    [[nodiscard]]
    Location start() const noexcept {
        return node->start;
    }

    /**
     * Retrieves the end location of the definition.
     * @return End location of the definition.
     */
    [[nodiscard]]
    Location end() const noexcept {
        return node->end;
    }
};

/**
 * Represents a data structure that manages symbols.
 */
class SymbolTable {
private:
    std::vector<std::unique_ptr<Symbol>> _symbols;

public:
    SymbolTable() = default;

    SymbolTable(const SymbolTable&) = default;
    SymbolTable& operator=(const SymbolTable&) = default;

    SymbolTable(SymbolTable&&) = default;
    SymbolTable& operator=(SymbolTable&&) = default;

    /**
     * Creates a `Symbol` instance.
     * @param symbol Symbol to create.
     * @return Pointer to the created symbol.
     */
    Symbol* make(Symbol symbol) {
        return _symbols.emplace_back(std::make_unique<Symbol>(std::move(symbol))).get();
    }

    /**
     * Moves all symbols from another symbol table into itself.
     * @param from Symbol table to move from.
     */
    void adopt(SymbolTable& from) {
        for (std::unique_ptr<Symbol>& p : from._symbols) {
            _symbols.push_back(std::move(p));
        }
        from._symbols.clear();
    }
};

/**
 * Represents a data structure that maps an AST node to a symbol.
 */
class NodeSymbolMap {
private:
    std::unordered_map<const Node*, Symbol*> _map;

public:
    NodeSymbolMap() = default;

    NodeSymbolMap(const NodeSymbolMap&) = default;
    NodeSymbolMap& operator=(const NodeSymbolMap&) = default;

    NodeSymbolMap(NodeSymbolMap&&) = default;
    NodeSymbolMap& operator=(NodeSymbolMap&&) = default;

    /**
     * Checks whether an AST node has a symbol.
     * @param node AST node to check.
     * @return `true` if the AST node has a symbol, false otherwise.
     */
    [[nodiscard]]
    bool hasSymbol(const Node* node) const {
        return _map.find(node) != _map.end();
    }

    /**
     * Retrieves the symbol of the AST node.
     * @param node AST node to retrieve symbol from.
     * @return Symbol or `nullptr` if the AST node doesn't have a symbol.
     */
    [[nodiscard]]
    Symbol* symbolOf(const Node* node) const {
        auto it = _map.find(node);
        return it == _map.end() ? nullptr : it->second;
    }

    /**
     * Sets the symbol of an AST node.
     * @param node AST node to set symbol.
     * @param symbol Symbol to set.
     */
    void set(const Node* node, Symbol* symbol) {
        _map[node] = symbol;
    }
};

} // Spark::FrontEnd
