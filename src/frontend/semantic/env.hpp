#pragma once

#include <unordered_map>

#include "frontend/ast/name.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

/**
 * Represents an environment that maps names to symbols.
 */
class Env {
private:
    std::unordered_map<std::string, Symbol*> _map;

public:
    void set(std::string name, Symbol* symbol) {
        _map.emplace(std::move(name), symbol);
    }

    Symbol* get(const std::string& name) const {
        auto it = _map.find(name);
        return it == _map.end() ? nullptr : it->second;
    }
};

} // Spark::FrontEnd
