#pragma once

#include <string_view>
#include <unordered_map>

#include "symbol.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a scope's binding environment, mapping name to symbols declared in the environment.
 */
class Env {
private:
    const Env* _parent;
    std::unordered_map<std::string_view, Symbol*> _map;

public:
    Env() : _parent(nullptr) { }
    explicit Env(const Env& parent) : _parent(&parent) { }

    [[nodiscard]]
    Symbol* get(std::string_view name) const {
        auto it = _map.find(name);
        return it != _map.end() ? it->second : nullptr;
    }

    void set(std::string_view name, Symbol* symbol) {
        _map.emplace(name, symbol);
    }

    void remove(std::string_view name) {
        _map.erase(name);
    }

    [[nodiscard]]
    Symbol* find(std::string_view name) const {
        auto it = _map.find(name);
        return it != _map.end() ? it->second : (_parent != nullptr ? _parent->find(name) : nullptr);
    }
};

} // Spark::FrontEnd
