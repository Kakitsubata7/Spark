#pragma once

#include <cstdint>
#include <memory>
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

    template <typename T>
    [[nodiscard]]
    bool is() const {
        return dynamic_cast<const T*>(this) != nullptr;
    }

    template <typename T>
    [[nodiscard]]
    T* as() {
        return dynamic_cast<T*>(this);
    }

    template <typename T>
    [[nodiscard]]
    const T* as() const {
        return dynamic_cast<const T*>(this);
    }
};

class LuaBlock;

class LuaFuncDef : public LuaNode {
private:
    SemanticFunc* _func;
    std::vector<Symbol*> _params;
    LuaBlock* _body;

public:
    LuaFuncDef(SemanticFunc* func, std::vector<Symbol*> params, LuaBlock* body) noexcept
        : _func(func), _params(std::move(params)), _body(body) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaBlock : public LuaNode {
private:
    std::vector<Symbol*> _locals;

    std::vector<std::pair<SemanticFunc*, LuaFuncDef*>> _fndefs;

    std::vector<LuaNode*> _nodes;

public:
    LuaBlock(std::vector<Symbol*> locals,
             std::vector<std::pair<SemanticFunc*, LuaFuncDef*>> funcDefs,
             std::vector<LuaNode*> nodes) noexcept
        : _locals(std::move(locals)), _fndefs(std::move(funcDefs)), _nodes(std::move(nodes)) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;

private:
    static void emitLocalVarDecls(std::ostringstream& oss,
                                  LuaNameMangler& mangler,
                                  const std::vector<Symbol*>& locals);

    static void emitFuncDecls(std::ostringstream& oss,
                              LuaNameMangler& mangler,
                              const std::vector<std::pair<SemanticFunc*, LuaFuncDef*>>& fndefs);

    static void emitFuncImpls(std::ostringstream& oss,
                              LuaNameMangler& mangler,
                              const std::vector<std::pair<SemanticFunc*, LuaFuncDef*>>& fndefs);

    static void emitStmt(std::ostringstream& oss, LuaNameMangler& mangler, LuaNode* node);

    friend class LuaBlockExpr;
};

class LuaExpr : public LuaNode { };

class LuaStmt : public LuaNode { };

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

class LuaBlockExpr : public LuaExpr {
private:
    LuaBlock* _body;

public:
    explicit LuaBlockExpr(LuaBlock* body) noexcept :
        _body(body) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaAssignStmt : public LuaStmt {
private:
    LuaExpr* _lhs;
    LuaExpr* _rhs;

public:
    LuaAssignStmt(LuaExpr* lhs, LuaExpr* rhs) noexcept
        : _lhs(lhs), _rhs(rhs) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaIfStmt : public LuaStmt {
private:
    LuaExpr* _cond;
    LuaBlock* _thenBody;
    LuaBlock* _elseBody;

public:
    LuaIfStmt(LuaExpr* cond, LuaBlock* thenBody, LuaBlock* elseBody) noexcept
        : _cond(cond), _thenBody(thenBody), _elseBody(elseBody) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaReturnStmt : public LuaStmt {
private:
    LuaExpr* _expr;

public:
    explicit LuaReturnStmt(LuaExpr* expr) noexcept
        : _expr(expr) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaWhileStmt : public LuaStmt {
private:
    LuaExpr* _cond;
    LuaBlock* _body;

public:
    LuaWhileStmt(LuaExpr* cond, LuaBlock* body) noexcept
        : _cond(cond), _body(body) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaBreakStmt : public LuaStmt {
public:
    LuaBreakStmt() noexcept = default;

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaContinueStmt : public LuaStmt {
public:
    LuaContinueStmt() noexcept = default;

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};



class LuaNodeTable {
private:
    std::vector<std::unique_ptr<LuaNode>> _nodes;

public:
    template <typename T, typename... Args>
    T* makeType(Args&&... args) {
        std::unique_ptr<T> up = std::make_unique<T>(std::forward<Args>(args)...);
        T* p = up.get();
        _nodes.emplace_back(std::move(up));
        return p;
    }
};

} // Spark::FrontEnd
