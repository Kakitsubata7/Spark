#pragma once

#include <cassert>
#include <string_view>
#include <unordered_map>

#include "symbol.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a scope's binding environment, mapping name to symbols declared in the environment.
 */
class Env {
private:
    Env* _parent;
    std::unordered_map<std::string, Symbol*> _map;

public:
    Env() : _parent(nullptr) { }
    explicit Env(Env& parent) : _parent(&parent) { }

    Env(const Env& other) = delete;
    Env& operator=(const Env& other) = delete;

    Env(Env&& other) = default;
    Env& operator=(Env&& other) = default;

    [[nodiscard]]
    bool hasParent() const noexcept {
        return _parent != nullptr;
    }

    [[nodiscard]]
    Env& parent() noexcept {
        assert(_parent != nullptr);
        return *_parent;
    }

    [[nodiscard]]
    const Env& parent() const noexcept {
        assert(_parent != nullptr);
        return *_parent;
    }

    [[nodiscard]]
    Symbol* get(const std::string& name) {
        auto it = _map.find(name);
        return it != _map.end() ? it->second : nullptr;
    }

    void set(const std::string& name, Symbol* symbol) {
        assert(symbol != nullptr);
        _map[name] = symbol;
    }

    void erase(const std::string& name) {
        _map.erase(name);
    }

    [[nodiscard]]
    Symbol* lookup(const std::string& name) const {
        auto it = _map.find(name);
        return it != _map.end() ? it->second : (_parent != nullptr ? _parent->lookup(name) : nullptr);
    }

    [[nodiscard]]
    std::vector<Symbol*> getSymbols() const {
        std::vector<Symbol*> symbols;
        symbols.reserve(_map.size());

        for (const auto& pair : _map) {
            symbols.push_back(pair.second);
        }

        return symbols;
    }
};

} // Spark::FrontEnd
