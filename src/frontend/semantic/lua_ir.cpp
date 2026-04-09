#include "lua_ir.hpp"

#include <cassert>
#include <iomanip>
#include <sstream>

namespace Spark::FrontEnd {

const std::string& LuaNameMangler::mangle(const Symbol* symbol) {
    assert(symbol != nullptr);

    // Check cached name
    if (std::string* name = find(symbol)) {
        return *name;
    }

    // Mangle new name
    std::string name;
    name += "v_";
    name += std::to_string(getNextSymbolId());
    return add(symbol, std::move(name));
}

const std::string& LuaNameMangler::mangle(const SemanticFunc* func) {
    assert(func != nullptr);

    // Check cached name
    if (std::string* name = find(func)) {
        return *name;
    }

    // Mangle new name
    std::string name;
    name += "f_";
    name += std::to_string(getNextFuncId());
    return add(func, std::move(name));
}

std::string* LuaNameMangler::find(const void* p) {
    if (auto it = _map.find(p); it != _map.end()) {
        return &it->second;
    }
    return nullptr;
}

const std::string& LuaNameMangler::add(const void* p, std::string name) {
    auto [it, inserted] = _map.emplace(p, std::move(name));
    assert(inserted);
    return it->second;
}

uint64_t LuaNameMangler::getNextSymbolId() noexcept {
    uint64_t current = _symbolId++;
    assert(_symbolId > current && "`LuaNameMangler` runs out of symbol ID");
    return current;
}

uint64_t LuaNameMangler::getNextFuncId() noexcept {
    uint64_t current = _funcId++;
    assert(_funcId > current && "`LuaNameMangler` runs out of function ID");
    return current;
}



std::string LuaIntExpr::emit(LuaNameMangler& mangler) const {
    return std::to_string(_value);
}

std::string LuaRealExpr::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << std::setprecision(17) << _value;
    return oss.str();
}

std::string LuaBoolExpr::emit(LuaNameMangler& mangler) const {
    return _value ? "true" : "false";
}

std::string LuaStringExpr::emit(LuaNameMangler& mangler) const {
    std::string s;
    s += '"';
    s += _value;
    s += '"';
    return s;
}

std::string LuaVoidExpr::emit(LuaNameMangler& mangler) const {
    return "nil";
}

std::string LuaNilExpr::emit(LuaNameMangler& mangler) const {
    return "nil";
}

std::string LuaVarExpr::emit(LuaNameMangler& mangler) const {
    return mangler.mangle(_symbol);
}

std::string LuaFuncRefExpr::emit(LuaNameMangler& mangler) const {
    return mangler.mangle(_func);
}

std::string LuaCallExpr::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << _callee->emit(mangler) << '(';
    for (size_t i = 0; i < _args.size(); ++i) {
        oss << _args[i]->emit(mangler);
        if (i != _args.size() - 1) {
            oss << ", ";
        }
    }
    oss << ')';
    return oss.str();
}

std::string LuaMemberAccessExpr::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    std::string base = _base->emit(mangler);
    if (auto* symbol = std::get_if<Symbol*>(&_member)) {
        oss << base << '.' << mangler.mangle(*symbol);
    } else if (auto* func = std::get_if<SemanticFunc*>(&_member)) {
        oss << "(function(...) "
                << "return " << mangler.mangle(*func) << '(' << base << ", ...)"
            << " end)";
    } else {
        assert(false && "invalid `std::variant<Symbol*, SemanticFunc*>` value");
    }
    return oss.str();
}

} // Spark::FrontEnd
