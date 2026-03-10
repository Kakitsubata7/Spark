#pragma once

#include <string_view>

#include "env.hpp"
#include "frontend/ast.hpp"
#include "semantic_visitor.hpp"
#include "symbol.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

class PatternDeclarator : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    NodeSymbolMap& _nodeSymbolMap;

    Env& _env;

    SymbolKind _kind;
    bool _isReassignable;

    Diagnostics& _diagnostics;

public:
    PatternDeclarator(SymbolTable& symbolTable,
                      NodeSymbolMap& nodeSymbolMap,
                      Env& env,
                      SymbolKind kind,
                      bool isReassignable,
                      Diagnostics& diagnostics) noexcept
        : _symbolTable(symbolTable), _nodeSymbolMap(nodeSymbolMap), _env(env), _kind(kind),
          _isReassignable(isReassignable), _diagnostics(diagnostics) { }

    void visit(BindingPattern* pattern) override;
    void visit(TuplePattern* pattern) override;
    void visit(CollectionPattern* pattern) override;
    void visit(RecordPattern* pattern) override;

private:
    Symbol* declare(Name* node);

    void redeclareError(Location start,
                        Location end,
                        std::string_view name,
                        Location prevStart,
                        Location prevEnd) noexcept;
};

class NameDeclarator : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    NodeSymbolMap& _nodeSymbolMap;

    Env& _env;

    Diagnostics& _diagnostics;

public:
    NameDeclarator(SymbolTable& symbolTable,
                   NodeSymbolMap& nodeSymbolMap,
                   Env& env,
                   Diagnostics& diagnostics) noexcept
        : _symbolTable(symbolTable), _nodeSymbolMap(nodeSymbolMap), _env(env), _diagnostics(diagnostics) { }

    void visit(VarDefStmt* vardef) override;
    void visit(FnDefStmt* fndef) override;
};



class NameResolver : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    NodeSymbolMap& _nodeSymbolMap;

    const Env& _env;

    Diagnostics& _diagnostics;

public:
    NameResolver(SymbolTable& symbolTable, NodeSymbolMap& nodeSymbolMap, Env& env, Diagnostics& diagnostics) noexcept
        : _symbolTable(symbolTable), _nodeSymbolMap(nodeSymbolMap), _env(env), _diagnostics(diagnostics) { }

    void visit(NameExpr* ident) override;

private:
    Symbol* resolve(const Name* node);

    void cannotFindError(Location start, Location end, std::string_view name);
};



class SemanticResolver : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    NodeSymbolMap& _nodeSymbolMap;

    Env& _globalEnv;
    std::vector<Env> _envStack;

    Diagnostics& _diagnostics;

public:
    SemanticResolver(SymbolTable& symbolTable,
                     NodeSymbolMap& nodeSymbolMap,
                     Env& globalEnv,
                     Diagnostics& diagnostics)
        : _symbolTable(symbolTable), _nodeSymbolMap(nodeSymbolMap), _globalEnv(globalEnv), _diagnostics(diagnostics) { }

private:
    void visit(BlockExpr* block) override;
    void visit(IfThenExpr* ifthen) override;
    void visit(BinaryExpr* binary) override;
    void visit(PrefixExpr* prefix) override;
    void visit(PostfixExpr* postfix) override;
    void visit(NameExpr* ident) override;

    void visit(VarDefStmt* vardef) override;
    void visit(FnDefStmt* fndef) override;
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

    void resolve(Node* node) {
        assert(node != nullptr);
        node->accept(*this);
    }

    static bool isHoistedDeclarative(const Node* node) noexcept;
};

} // Spark::FrontEnd
