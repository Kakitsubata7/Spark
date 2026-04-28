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
    const std::string& get(const Symbol* symbol);
    const std::string& get(const SemanticFunc* func);

    void set(const Symbol* symbol, std::string name);
    void set(const SemanticFunc* func, std::string name);

private:
    std::string* find(const void* p);
    const std::string& add(const void* p, std::string name);

    [[nodiscard]]
    uint64_t getNextSymbolId() noexcept;

    [[nodiscard]]
    uint64_t getNextFuncId() noexcept;

    void set(const void* p, std::string name);
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

class LuaExpr : public LuaNode { };

class LuaNone : public LuaExpr {
public:
    explicit LuaNone() noexcept = default;

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaBody;

class LuaFuncDef : public LuaNode {
private:
    SemanticFunc* _func;
    std::vector<Symbol*> _params;
    LuaBody* _body;

public:
    LuaFuncDef(SemanticFunc* func, std::vector<Symbol*> params, LuaBody* body) noexcept
        : _func(func), _params(std::move(params)), _body(body) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaBody : public LuaNode {
private:
    std::vector<Symbol*> _locals;
    std::vector<LuaNode*> _nodes;

public:
    LuaBody(std::vector<Symbol*> locals,
            std::vector<LuaNode*> nodes) noexcept
        : _locals(std::move(locals)), _nodes(std::move(nodes)) { }

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

    friend class LuaBlock;
};

class LuaInt : public LuaExpr {
private:
    int64_t _value;

public:
    explicit LuaInt(int64_t value) noexcept
        : _value(value) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaReal : public LuaExpr {
private:
    double _value;

public:
    explicit LuaReal(double value) noexcept
        : _value(value) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaBool : public LuaExpr {
private:
    bool _value;

public:
    explicit LuaBool(bool value) noexcept
        : _value(value) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaString : public LuaExpr {
private:
    std::string _value;

public:
    explicit LuaString(std::string value) noexcept
        : _value(std::move(value)) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaVoid : public LuaExpr {
public:
    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaNil : public LuaExpr {
public:
    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaVarRef : public LuaExpr {
private:
    Symbol* _symbol;

public:
    explicit LuaVarRef(Symbol* symbol) noexcept
        : _symbol(symbol) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaFuncRef : public LuaExpr {
private:
    SemanticFunc* _func;

public:
    explicit LuaFuncRef(SemanticFunc* func) noexcept
        : _func(func) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaCall : public LuaExpr {
private:
    LuaNode* _callee;
    std::vector<LuaNode*> _args;

public:
    LuaCall(LuaNode* callee, std::vector<LuaNode*> args) noexcept
        : _callee(callee), _args(std::move(args)) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaMemberAccess : public LuaExpr {
private:
    LuaNode* _base;
    std::variant<Symbol*, SemanticFunc*> _member;

public:
    LuaMemberAccess(LuaNode* base, Symbol* member) noexcept
        : _base(base), _member(member) { }

    LuaMemberAccess(LuaNode* base, SemanticFunc* member) noexcept
        : _base(base), _member(member) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaBlock : public LuaExpr {
private:
    LuaBody* _body;

public:
    explicit LuaBlock(LuaBody* body) noexcept :
        _body(body) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaAssign : public LuaNode {
private:
    LuaNode* _lhs;
    LuaNode* _rhs;

public:
    LuaAssign(LuaNode* lhs, LuaNode* rhs) noexcept
        : _lhs(lhs), _rhs(rhs) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaIf : public LuaNode {
private:
    LuaNode* _cond;
    LuaBody* _thenBody;
    /* nullable */ LuaBody* _elseBody;

public:
    LuaIf(LuaNode* cond, LuaBody* thenBody, LuaBody* elseBody = nullptr) noexcept
        : _cond(cond), _thenBody(thenBody), _elseBody(elseBody) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaReturn : public LuaNode {
private:
    LuaNode* _ret;

public:
    explicit LuaReturn(LuaNode* ret) noexcept
        : _ret(ret) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaWhile : public LuaNode {
private:
    LuaNode* _cond;
    LuaBody* _body;

public:
    LuaWhile(LuaNode* cond, LuaBody* body) noexcept
        : _cond(cond), _body(body) { }

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaBreak : public LuaNode {
public:
    LuaBreak() noexcept = default;

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};

class LuaContinue : public LuaNode {
public:
    LuaContinue() noexcept = default;

    [[nodiscard]]
    std::string emit(LuaNameMangler& mangler) const override;
};



class LuaNodeTable {
private:
    std::vector<std::unique_ptr<LuaNode>> _nodes;

public:
    template <typename T, typename... Args>
    T* make(Args&&... args) {
        std::unique_ptr<T> up = std::make_unique<T>(std::forward<Args>(args)...);
        T* p = up.get();
        _nodes.emplace_back(std::move(up));
        return p;
    }
};



class LuaEmitter {
private:
    LuaNameMangler& _mangler;

    std::vector<std::string> _sources;

public:
    explicit LuaEmitter(LuaNameMangler& mangler) noexcept
        : _mangler(mangler) { }

    void registerSource(std::string source);

    [[nodiscard]]
    std::string emit(LuaNode* top) const;
};

} // Spark::FrontEnd
