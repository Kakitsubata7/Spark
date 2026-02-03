#pragma once

#include <unordered_map>

#include "frontend/ast/name.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a lexical binding environment during name resolution.
 */
class Env {
private:
    std::unordered_map<InternedNameValue, Symbol*> _map;

public:
    Symbol* get(InternedNameValue name) const noexcept {
        if (_map.find(name) == _map.end()) {
            return nullptr;
        }
        return _map.at(name);
    }

    void set(InternedNameValue name, Symbol* symbol) {
        _map[name] = symbol;
    }
};

} // Spark::FrontEnd
