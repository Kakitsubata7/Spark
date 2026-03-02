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
    std::unordered_map<std::string_view, Symbol*> _map;

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
    Env& parent() const noexcept {
        assert(_parent != nullptr);
        return *_parent;
    }

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
