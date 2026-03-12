#pragma once

#include <string_view>

#include "env.hpp"
#include "frontend/ast.hpp"
#include "symbol.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a visitor that creates symbol(s) for a name or a pattern and binds them to the given environment.
 */
class NameDeclarator : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    NodeSymbolMap& _nodeSymbolMap;

    Env& _env;

    SymbolKind _kind;
    bool _isReassignable;

    Diagnostics& _diagnostics;

public:
    NameDeclarator(SymbolTable& symbolTable,
                   NodeSymbolMap& nodeSymbolMap,
                   Env& env,
                   SymbolKind kind,
                   bool isReassignable,
                   Diagnostics& diagnostics) noexcept
        : _symbolTable(symbolTable), _nodeSymbolMap(nodeSymbolMap), _env(env), _kind(kind),
          _isReassignable(isReassignable), _diagnostics(diagnostics) { }

    void visit(Name* node) override;
    void visit(BindingPattern* pattern) override;
    void visit(TuplePattern* pattern) override;
    void visit(CollectionPattern* pattern) override;
    void visit(RecordPattern* pattern) override;

private:
    void redeclareError(Location start,
                        Location end,
                        std::string_view name,
                        Location prevStart,
                        Location prevEnd) noexcept;
};



/**
 * Represents a visitor that declares names and resolves name usages, creating environment mappings for each scope.
 */
class NameResolver : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    NodeSymbolMap& _nodeSymbolMap;

    Env& _globalEnv;
    std::vector<Env> _envStack;

    Diagnostics& _diagnostics;

public:
    NameResolver(SymbolTable& symbolTable,
                 NodeSymbolMap& nodeSymbolMap,
                 Env& globalEnv,
                 Diagnostics& diagnostics)
        : _symbolTable(symbolTable), _nodeSymbolMap(nodeSymbolMap), _globalEnv(globalEnv), _diagnostics(diagnostics) { }

private:
    void visit(Name* node) override;
    void visit(FnParam* param) override;

    void visit(IfThenExpr* ifthen) override;
    void visit(BlockExpr* block) override;
    void visit(BinaryExpr* binary) override;
    void visit(PrefixExpr* prefix) override;
    void visit(PostfixExpr* postfix) override;
    void visit(NameExpr* ident) override;

    void visit(VarDefStmt* vardef) override;
    void visit(FnDefStmt* fndef) override;
    void visit(TypeDefStmt* tdef) override;
    void visit(IfStmt* ifstmt) override;
    void visit(WhileStmt* w) override;
    void visit(DoWhileStmt* dowhile) override;
    void visit(ReturnStmt* ret) override;

    Env& currentEnv() noexcept {
        return _envStack.empty() ? _globalEnv : _envStack.back();
    }

    void pushEnv() {
        _envStack.emplace_back(currentEnv());
    }

    void popEnv() {
        assert(!_envStack.empty());
        _envStack.pop_back();
    }

    void declare(Name* node, Env& env, SymbolKind kind, bool isReassignable);
    void declare(Pattern* pattern, Env& env, SymbolKind kind, bool isReassignable);

    static bool isReassignable(const VarModifier* varmod) noexcept;
    static bool isHoistedDeclarative(const Node* node) noexcept;

    void cannotFindError(Location start, Location end, std::string_view name);
};

} // Spark::FrontEnd
