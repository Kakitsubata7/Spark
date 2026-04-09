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



std::string LuaNone::emit(LuaNameMangler& mangler) const {
    return "_";
}

std::string LuaFuncDef::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << mangler.mangle(_func) << " = function(";
    for (size_t i = 0; i < _params.size(); ++i) {
        oss << mangler.mangle(_params[i]);
        if (i != _params.size() - 1) {
            oss << ", ";
        }
    }
    oss << ")\n"
            << _body->emit(mangler)
        << "end";
    return oss.str();
}

std::string LuaBody::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;

    if (!_locals.empty()) {
        // Local variable declarations
        emitLocalVarDecls(oss, mangler, _locals);
        oss << '\n';
    }

    // Statements
    for (size_t i = 0; i < _nodes.size(); ++i) {
        emitStmt(oss, mangler, _nodes[i]);
        if (i != _nodes.size() - 1) {
            oss << '\n';
        }
    }

    return oss.str();
}

void LuaBody::emitLocalVarDecls(std::ostringstream& oss,
                                 LuaNameMangler& mangler,
                                 const std::vector<Symbol*>& locals) {
    for (size_t i = 0; i < locals.size(); ++i) {
        oss << "local " << mangler.mangle(locals[i]);
        if (i != locals.size() - 1) {
            oss << '\n';
        }
    }
}

void LuaBody::emitFuncDecls(std::ostringstream& oss,
                             LuaNameMangler& mangler,
                             const std::vector<std::pair<SemanticFunc*, LuaFuncDef*>>& fndefs) {
    for (size_t i = 0; i < fndefs.size(); ++i) {
        auto [func, _] = fndefs[i];
        oss << "local " << mangler.mangle(func);
        if (i != fndefs.size() - 1) {
            oss << '\n';
        }
    }
}

void LuaBody::emitFuncImpls(std::ostringstream& oss,
                             LuaNameMangler& mangler,
                             const std::vector<std::pair<SemanticFunc*, LuaFuncDef*>>& fndefs) {
    for (size_t i = 0; i < fndefs.size(); ++i) {
        auto [func, fndef] = fndefs[i];
        oss << mangler.mangle(func) << " = " << fndef->emit(mangler);
        if (i != fndefs.size() - 1) {
            oss << '\n';
        }
    }
}

void LuaBody::emitStmt(std::ostringstream& oss, LuaNameMangler& mangler, LuaNode* node) {
    if (LuaExpr* expr = node->as<LuaExpr>()) {
        oss << "_ = " << expr->emit(mangler);
    } else {
        oss << node->emit(mangler);
    }
}

std::string LuaInt::emit(LuaNameMangler& mangler) const {
    return std::to_string(_value);
}

std::string LuaReal::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << std::setprecision(17) << _value;
    return oss.str();
}

std::string LuaBool::emit(LuaNameMangler& mangler) const {
    return _value ? "true" : "false";
}

std::string LuaString::emit(LuaNameMangler& mangler) const {
    std::string s;
    s += '"';
    for (char c : _value) {
        switch (c) {
            case '\n':
                s += "\\n";
                break;
            case '\t':
                s += "\\t";
                break;
            case '\r':
                s += "\\r";
                break;
            case '\\':
                s += "\\\\";
                break;
            case '"':
                s += "\\\"";
                break;
            case '\'':
                s += "\\'";
                break;
            default:
                s += c;
                break;
        }
    }
    s += '"';
    return s;
}

std::string LuaVoid::emit(LuaNameMangler& mangler) const {
    return "_";
}

std::string LuaNil::emit(LuaNameMangler& mangler) const {
    return "nil";
}

std::string LuaVarRef::emit(LuaNameMangler& mangler) const {
    return mangler.mangle(_symbol);
}

std::string LuaFuncRef::emit(LuaNameMangler& mangler) const {
    return mangler.mangle(_func);
}

std::string LuaCall::emit(LuaNameMangler& mangler) const {
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

std::string LuaMemberAccess::emit(LuaNameMangler& mangler) const {
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

std::string LuaBlock::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;

    oss << "(function()\n";

    if (!_body->_locals.empty()) {
        // Local variable declarations
        LuaBody::emitLocalVarDecls(oss, mangler, _body->_locals);
        oss << '\n';
    }

    // Statements
    if (_body->_nodes.empty()) {
        oss << "return nil";
    } else {
        for (size_t i = 0; i < _body->_nodes.size() - 1; ++i) {
            LuaBody::emitStmt(oss, mangler, _body->_nodes[i]);
            oss << '\n';
        }

        LuaNode* last = _body->_nodes.back();
        if (last->is<LuaExpr>()) {
            oss << "return " << last->emit(mangler);
        } else {
            oss << last->emit(mangler) << '\n'
                << "return " << LuaVoid().emit(mangler);
        }
    }

    oss << "\nend)()";

    return oss.str();
}

std::string LuaAssign::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << _lhs->emit(mangler) << " = " << _rhs->emit(mangler);
    return oss.str();
}

std::string LuaIf::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << "if " << _cond->emit(mangler) << " then\n"
            << _thenBody->emit(mangler) << '\n';
    if (_elseBody == nullptr) {
        oss << "end";
    } else {
        oss << "else\n"
                << _elseBody->emit(mangler) << '\n'
            << "end";
    }
    return oss.str();
}

std::string LuaReturn::emit(LuaNameMangler& mangler) const {
    return "return " + _ret->emit(mangler);
}

std::string LuaWhile::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << "while " << _cond->emit(mangler) << " do\n"
            << _body->emit(mangler) << '\n'
            << "::continue::\n"
        << "end";
    return oss.str();
}

std::string LuaBreak::emit(LuaNameMangler& mangler) const {
    return "break";
}

std::string LuaContinue::emit(LuaNameMangler& mangler) const {
    return "goto continue";
}


LuaEmitter::LuaEmitter(LuaNameMangler& mangler, LuaNodeTable& table) noexcept
    : _mangler(mangler), _table(table) {
    // TODO: _top = _table.make<LuaBlock>();
}

std::string LuaEmitter::emit() const {
    return _top->emit(_mangler);
}

} // Spark::FrontEnd
