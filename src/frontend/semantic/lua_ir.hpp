#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "semantic_func.hpp"
#include "symbol.hpp"

namespace Spark::FrontEnd {

class LuaNameMangler {
private:
    std::unordered_map<const void*, std::string> _map;

    uint64_t _symbolId = 0;
    uint64_t _funcId = 0;

public:
    const std::string& mangle(const Symbol* symbol);
    const std::string& mangle(const SemanticFunc* func);

private:
    std::string* find(const void* p);
    const std::string& add(const void* p, std::string name);

    [[nodiscard]]
    uint64_t getNextSymbolId() noexcept;

    [[nodiscard]]
    uint64_t getNextFuncId() noexcept;
};



class LuaNode {
public:
    virtual ~LuaNode() = default;

    [[nodiscard]]
    virtual std::string emit(LuaNameMangler& mangler) const = 0;
};

class LuaBlock {

};

class LuaExpr : public LuaNode {
public:
    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaIntExpr : public LuaExpr {
private:
    int64_t _value;

public:
    explicit LuaIntExpr(int64_t value) noexcept
        : _value(value) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaRealExpr : public LuaExpr {
private:
    double _value;

public:
    explicit LuaRealExpr(double value) noexcept
        : _value(value) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaBoolExpr : public LuaExpr {
private:
    bool _value;

public:
    explicit LuaBoolExpr(bool value) noexcept
        : _value(value) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaStringExpr : public LuaExpr {
private:
    std::string _value;

public:
    explicit LuaStringExpr(std::string value) noexcept
        : _value(std::move(value)) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaVoidExpr : public LuaExpr {
public:
    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaNilExpr : public LuaExpr {
public:
    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaVarExpr : public LuaExpr {
private:
    Symbol* _symbol;

public:
    explicit LuaVarExpr(Symbol* symbol) noexcept
        : _symbol(symbol) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaFuncRefExpr : public LuaExpr {
private:
    SemanticFunc* _func;

public:
    explicit LuaFuncRefExpr(SemanticFunc* func) noexcept
        : _func(func) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaCallExpr : public LuaExpr {
private:
    LuaExpr* _callee;
    std::vector<LuaExpr*> _args;

public:
    LuaCallExpr(LuaExpr* callee, std::vector<LuaExpr*> args) noexcept
        : _callee(callee), _args(std::move(args)) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaMemberAccessExpr : public LuaExpr {
private:
    LuaExpr* _base;
    std::variant<Symbol*, SemanticFunc*> _member;

public:
    LuaMemberAccessExpr(LuaExpr* base, Symbol* member) noexcept
        : _base(base), _member(member) { }

    LuaMemberAccessExpr(LuaExpr* base, SemanticFunc* member) noexcept
        : _base(base), _member(member) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

} // Spark::FrontEnd
