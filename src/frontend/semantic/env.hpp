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
    void define(InternedNameValue name, Symbol* symbol) {
        _map[name] = symbol;
    }

    Symbol* get(InternedNameValue name) const {
        return _map.at(name);
    }
};

} // Spark::FrontEnd
