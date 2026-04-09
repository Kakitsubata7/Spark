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



std::string LuaFuncDef::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << "function(";
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

std::string LuaBlock::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;

    if (!_locals.empty()) {
        // Local variable declarations
        emitLocalVarDecls(oss, mangler, _locals);
        oss << '\n';
    }

    if (!_fndefs.empty()) {
        // Function declarations
        emitFuncDecls(oss, mangler, _fndefs);
        oss << '\n';

        // Function implementations
        emitFuncImpls(oss, mangler, _fndefs);
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

void LuaBlock::emitLocalVarDecls(std::ostringstream& oss,
                                 LuaNameMangler& mangler,
                                 const std::vector<Symbol*>& locals) {
    for (size_t i = 0; i < locals.size(); ++i) {
        oss << "local " << mangler.mangle(locals[i]);
        if (i != locals.size() - 1) {
            oss << '\n';
        }
    }
}

void LuaBlock::emitFuncDecls(std::ostringstream& oss,
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

void LuaBlock::emitFuncImpls(std::ostringstream& oss,
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

void LuaBlock::emitStmt(std::ostringstream& oss, LuaNameMangler& mangler, LuaNode* node) {
    if (LuaExpr* expr = node->as<LuaExpr>()) {
        oss << "_ = " << expr->emit(mangler);
    } else if (LuaStmt* stmt = node->as<LuaStmt>()) {
        oss << stmt->emit(mangler);
    }
    // Ignores other types of nodes
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

std::string LuaBlockExpr::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;

    oss << "(function()\n";

    if (!_body->_locals.empty()) {
        // Local variable declarations
        LuaBlock::emitLocalVarDecls(oss, mangler, _body->_locals);
        oss << '\n';
    }

    if (!_body->_fndefs.empty()) {
        // Function declarations
        LuaBlock::emitFuncDecls(oss, mangler, _body->_fndefs);
        oss << '\n';

        // Function implementations
        LuaBlock::emitFuncImpls(oss, mangler, _body->_fndefs);
        oss << '\n';
    }

    // Statements
    if (_body->_nodes.empty()) {
        oss << "return nil";
    } else {
        for (size_t i = 0; i < _body->_nodes.size() - 1; ++i) {
            LuaBlock::emitStmt(oss, mangler, _body->_nodes[i]);
            oss << '\n';
        }
        if (LuaExpr* lastExpr = _body->_nodes.back()->as<LuaExpr>()) {
            oss << "return " << lastExpr->emit(mangler);
        } else if (LuaStmt* lastStmt = _body->_nodes.back()->as<LuaStmt>()) {
            oss << lastStmt->emit(mangler) << '\n'
                << "return " << LuaVoidExpr().emit(mangler);
        }
    }

    oss << "\nend)()";

    return oss.str();
}

std::string LuaAssignStmt::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << _lhs->emit(mangler) << " = " << _rhs->emit(mangler);
    return oss.str();
}

std::string LuaIfStmt::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << "if " << _cond->emit(mangler) << " then\n"
            << _thenBody->emit(mangler) << '\n'
        << "else\n"
            << _elseBody->emit(mangler) << '\n'
        << "end";
    return oss.str();
}

std::string LuaReturnStmt::emit(LuaNameMangler& mangler) const {
    return "return " + _expr->emit(mangler);
}

std::string LuaWhileStmt::emit(LuaNameMangler& mangler) const {
    std::ostringstream oss;
    oss << "while " << _cond->emit(mangler) << " do\n"
            << _body->emit(mangler) << '\n'
            << "::continue::\n"
        << "end";
    return oss.str();
}

std::string LuaBreakStmt::emit(LuaNameMangler& mangler) const {
    return "break";
}

std::string LuaContinueStmt::emit(LuaNameMangler& mangler) const {
    return "goto continue";
}

} // Spark::FrontEnd
