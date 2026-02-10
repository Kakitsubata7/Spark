#pragma once

#include <cassert>
#include <unordered_map>

#include "frontend/ast/name.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a lexical binding environment during name resolution.
 */
class Env {
private:
    struct Decl {
        const Symbol* symbol;
        bool isVisible;
    };

    std::unordered_map<InternedNameValue, Decl> _declMap;

public:
    Env() = default;

    Env(const Env& other) = default;
    Env& operator=(const Env& other) = default;

    Env(Env&& other) = default;
    Env& operator=(Env&& other) = default;

    /**
     * Declares a symbol to this environment with its visibility.
     * @param symbol Symbol to declare.
     * @param isVisible Whether the symbol is visible or not (default is true).
     */
    void declare(const Symbol* symbol, bool isVisible = true) {
        InternedNameValue name = symbol->name();
        _declMap.insert_or_assign(name, Decl{symbol, isVisible});
    }

    /**
     * Finds the symbol by its name. If no symbol with the name was found, `nullptr` is returned.
     * @param name Name to find.
     * @return Symbol or `nullptr` if no symbol with the name was found.
     */
    [[nodiscard]]
    const Symbol* find(InternedNameValue name) const {
        auto it = _declMap.find(name);
        return it == _declMap.end() ? nullptr : it->second.symbol;
    }

    /**
     * Checks whether the symbol with the name is visible or not. If the name is not associated with a symbol, `false`
     * is returned.
     * @param name Name of the symbol to check for.
     * @return `true` if symbol is visible, `false` otherwise or if the environment doesn't have a symbol with the name.
     */
    [[nodiscard]]
    bool isVisible(InternedNameValue name) const {
        auto it = _declMap.find(name);
        return it != _declMap.end() && it->second.isVisible;
    }

    /**
     * Sets the visibility of a name in the environment.
     * The name has to exist in the environment, otherwise assertion failure (debug mode) or undefined behavior.
     * @param name Name to set visibility.
     * @param isVisible Visibility.
     */
    void setVisible(InternedNameValue name, bool isVisible) {
        auto it = _declMap.find(name);
        assert(it != _declMap.end());
        it->second.isVisible = isVisible;
    }
};

} // Spark::FrontEnd
