#pragma once

#include <string_view>
#include <utility>
#include <vector>

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
        auto it = _map.find(std::string{name});
        return it == _map.end() ? nullptr : it->second;
    }
};

/**
 * Represents name resolution state context.
 * It stores the global environment and maps symbols to their sub-environment if they have one.
 */
class NameResolveContext {
private:
    SymbolTable& _symbolTable;

    Env _globalEnv;

    std::unordered_map<const Symbol*, std::unique_ptr<Env>> _subEnvMap;

public:
    explicit NameResolveContext(SymbolTable& symbolTable) noexcept : _symbolTable(symbolTable) { }

    [[nodiscard]]
    SymbolTable& symbolTable() const noexcept { return _symbolTable; }

    [[nodiscard]]
    Env& globalEnv() noexcept { return _globalEnv; }

    Symbol* makeSymbol(Symbol symbol) {
        return _symbolTable.make(symbol);
    }

    Env& getOrCreateSubEnv(const Symbol* symbol) {
        assert(symbol != nullptr);

        auto it = _subEnvMap.find(symbol);
        if (it == _subEnvMap.end()) {
            return *_subEnvMap.emplace(symbol, std::make_unique<Env>()).first->second;
        }
        return *it->second;
    }

    Env& getSubEnv(const Symbol* symbol) {
        assert(symbol != nullptr);

        auto it = _subEnvMap.find(symbol);
        assert(it != _subEnvMap.end());
        return *it->second;
    }

    [[nodiscard]]
    bool hasSubEnv(const Symbol* symbol) {
        assert(symbol != nullptr);
        return _subEnvMap.find(symbol) != _subEnvMap.end();
    }
};

class Declarator : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    Env& _env;
    SymbolKind _kind;
    bool _isReassignable;
    Diagnostics& _diagnostics;

public:
    Declarator(SymbolTable& symbolTable,
                      Env& env,
                      SymbolKind kind,
                      bool isReassignable,
                      Diagnostics& diagnostics) noexcept
        : _symbolTable(symbolTable), _env(env), _kind(kind), _isReassignable(isReassignable),
          _diagnostics(diagnostics) { }

    static void declare(Name* name,
                        SymbolTable& symbolTable,
                        Env& env,
                        SymbolKind kind,
                        bool isReassignable,
                        Diagnostics& diagnostics);

    static void declare(Pattern* pattern,
                        SymbolTable& symbolTable,
                        Env& env,
                        SymbolKind kind,
                        bool isReassignable,
                        Diagnostics& diagnostics);

    void visit(Name* name) override;
    void visit(BindingPattern* pattern) override;
    void visit(TuplePattern* pattern) override;
    void visit(CollectionPattern* pattern) override;
    void visit(RecordPattern* pattern) override;

private:
    void diagnostic(Diagnostic diagnostic) {
        _diagnostics.add(std::move(diagnostic));
    }
};

class StmtDeclarator : public NodeVisitor {
private:
    NameResolveContext& _ctx;
    Env& _env;
    Diagnostics& _diagnostics;

public:
    StmtDeclarator(NameResolveContext& ctx, Env& env, Diagnostics& diagnostics) noexcept
        : _ctx(ctx), _env(env), _diagnostics(diagnostics) { }

    static void declare(Stmt* stmt, NameResolveContext& ctx, Env& env, Diagnostics& diagnostics);

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
    NameResolveContext& _ctx;
    Diagnostics& _diagnostics;

public:
    GlobalNameResolver(NameResolveContext& ctx, Diagnostics& diagnostics) noexcept
        : _ctx(ctx), _diagnostics(diagnostics) { }

    static void resolve(const AST& ast, NameResolveContext& ctx, Diagnostics& diagnostics);

    void visit(ExportStmt* e) override;
};

/**
 * Resolves all lexical names and module names to symbols, ignoring globally exported names.
 */
class NameResolver : public NodeVisitor {
private:
    NameResolveContext& _ctx;
    Diagnostics& _diagnostics;

    std::vector<Env> _envStack;

public:
    explicit NameResolver(NameResolveContext& ctx, Diagnostics& diagnostics) noexcept
        : _ctx(ctx), _diagnostics(diagnostics) { }

    static void resolve(const AST& ast, NameResolveContext& ctx, Diagnostics& diagnostics);

    void visit(LambdaExpr* expr) override;
    void visit(IfThenExpr* expr) override;
    void visit(TryElseExpr* expr) override;
    void visit(MatchExpr* expr) override;
    void visit(TryCatchExpr* expr) override;
    void visit(ThrowExpr* expr) override;
    void visit(BlockExpr* expr) override;
    void visit(IsExpr* expr) override;
    void visit(AsExpr* expr) override;
    void visit(BinaryExpr* expr) override;
    void visit(PrefixExpr* expr) override;
    void visit(PostfixExpr* expr) override;
    void visit(MemberAccessExpr* expr) override;
    void visit(CallExpr* expr) override;
    void visit(SubscriptExpr* expr) override;
    void visit(NameExpr* expr) override;
    void visit(GlobalAccessExpr* expr) override;
    void visit(UpvalueExpr* expr) override;
    void visit(TupleExpr* expr) override;
    void visit(CollectionExpr* expr) override;
    void visit(TypeofExpr* expr) override;

    void visit(VarDefStmt* stmt) override;
    void visit(FnDefStmt* stmt) override;
    void visit(TypeDefStmt* stmt) override;
    void visit(CaseDefStmt* stmt) override;
    void visit(AssignStmt* stmt) override;
    void visit(IfStmt* stmt) override;
    void visit(WhileStmt* stmt) override;
    void visit(DoWhileStmt* stmt) override;
    void visit(ForStmt* stmt) override;
    void visit(ReturnStmt* stmt) override;
    void visit(ModuleStmt* stmt) override;
    void visit(ExportStmt* stmt) override;
    void visit(ImportStmt* stmt) override;
    void visit(ImportAllStmt* stmt) override;
    void visit(UndefineStmt* stmt) override;

private:
    Env& pushEnv() {
        return _envStack.emplace_back();
    }

    void popEnv() {
        if (!_envStack.empty()) {
            _envStack.pop_back();
        }
    }

    [[nodiscard]]
    Env& currentEnv() {
        assert(!_envStack.empty());
        return _envStack.back();
    }

    [[nodiscard]]
    Symbol* find(std::string_view name) {
        for (auto it = _envStack.rbegin(); it != _envStack.rend(); ++it) {
            Env& env = *it;
            if (Symbol* symbol = env.get(name)) {
                return symbol;
            }
        }
        return _ctx.globalEnv().get(name);
    }

    Symbol* resolveName(const Name* name, const Env& env, const std::optional<std::string_view>& base = std::nullopt);

    void diagnostic(Diagnostic diagnostic) {
        _diagnostics.add(std::move(diagnostic));
    }
};

} // Spark::FrontEnd
