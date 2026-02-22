#pragma once

#include <string_view>
#include <utility>

#include "frontend/ast.hpp"
#include "symbol.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

/**
 * Represents an environment that maps names to symbols during name resolution.
 */
class Env {
private:
    std::unordered_map<std::string, Symbol*> _map;

public:
    void set(std::string_view name, Symbol* symbol) {
        _map.emplace(std::string(name), symbol);
    }

    Symbol* get(std::string_view name) const {
        auto it = _map.find(std::string(name));
        return it == _map.end() ? nullptr : it->second;
    }
};

class NameResolveContext {
private:
    SymbolTable& _symbolTable;

    std::unordered_map<const Symbol*, std::unique_ptr<Env>> _moduleEnvMap;

public:
    explicit NameResolveContext(SymbolTable& symbolTable) noexcept : _symbolTable(symbolTable) { }

    Symbol* makeSymbol(Symbol symbol) {
        return _symbolTable.make(symbol);
    }

    Env* makeSubEnv(const Symbol* symbol) {
        return _moduleEnvMap.emplace(symbol, std::make_unique<Env>()).first->second.get();
    }

    [[nodiscard]]
    Env* getSubEnv(const Symbol* symbol) const {
        auto it = _moduleEnvMap.find(symbol);
        return it == _moduleEnvMap.end() ? nullptr : it->second.get();
    }
};

class PatternDeclarator : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    Env& _env;
    SymbolKind _kind;
    bool _isReassignable;
    Diagnostics& _diagnostics;

public:
    PatternDeclarator(SymbolTable& symbolTable,
                      Env& env,
                      SymbolKind kind,
                      bool isReassignable,
                      Diagnostics& diagnostics) noexcept
        : _symbolTable(symbolTable), _env(env), _kind(kind), _isReassignable(isReassignable),
          _diagnostics(diagnostics) { }

    static void declare(Pattern* pattern,
                        SymbolTable& symbolTable,
                        Env& env,
                        SymbolKind kind,
                        bool isReassignable,
                        Diagnostics& diagnostics);

    void visit(BindingPattern* pattern) override;
    void visit(TuplePattern* pattern) override;
    void visit(CollectionPattern* pattern) override;
    void visit(RecordPattern* pattern) override;

private:
    void diagnostic(Diagnostic diagnostic) {
        _diagnostics.add(std::move(diagnostic));
    }
};

class Declarator : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    Env& _env;
    Diagnostics& _diagnostics;

public:
    Declarator(SymbolTable& symbolTable, Env& env, Diagnostics& diagnostics) noexcept
        : _symbolTable(symbolTable), _env(env), _diagnostics(diagnostics) { }

    static void declare(Node* node, SymbolTable& symbolTable, Env& env, Diagnostics& diagnostics);

    void visit(MatchCase* c) override;

    void visit(VarDefStmt* vardef) override;
    void visit(FnDefStmt* fndef) override;
    void visit(TypeDefStmt* tdef) override;
    void visit(ModuleStmt* mdef) override;

private:
    void diagnostic(Diagnostic diagnostic) {
        _diagnostics.add(std::move(diagnostic));
    }
};

/**
 * Resolves all globally exported names to symbols and add them to the global environment.
 * If the name already exists in the given global environment, it will be overridden.
 */
class GlobalNameResolver : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    Env& _globalEnv;
    Diagnostics& _diagnostics;

public:
    GlobalNameResolver(SymbolTable& symbolTable, Env& globalEnv, Diagnostics& diagnostics) noexcept
        : _symbolTable(symbolTable), _globalEnv(globalEnv), _diagnostics(diagnostics) { }

    static void resolve(const AST& ast, SymbolTable& symbolTable, Env& globalEnv, Diagnostics& diagnostics);

    void visit(ExportStmt* e) override;
};

/**
 * Resolves all lexical names and module names to symbols, ignoring globally exported names.
 */
class NameResolver : public NodeVisitor {
private:
    const Env& _globalEnv;
    Diagnostics& _diagnostics;

public:
    explicit NameResolver(const Env& globalEnv, Diagnostics& diagnostics) noexcept
        : _globalEnv(globalEnv), _diagnostics(diagnostics) { }

    static void resolve(const AST& ast, const Env& globalEnv, Diagnostics& diagnostics);

    void visit(MemberAccessExpr* expr) override;
};

} // Spark::FrontEnd
