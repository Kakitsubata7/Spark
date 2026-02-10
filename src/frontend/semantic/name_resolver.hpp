#pragma once

#include <utility>
#include <vector>

#include "env.hpp"
#include "frontend/ast.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

/**
 * Represents the result of name resolution.
 */
struct NameResolveResult {
    Diagnostics diagnostics{};

    NameResolveResult() = default;
};

/**
 * Represents lexical symbol resolution.
 */
class NameResolver {
public:
    static NameResolveResult resolve(const AST& ast,
                                     SymbolTable& symTable,
                                     NodeSymbolMap& nodeSymMap,
                                     const Env& globalEnv = {});
};

/**
 * Represents a visitor that does name resolution.
 */
class NameResolveVisitor : public NodeVisitor {
private:
    SymbolTable& _symTable;
    NodeSymbolMap& _nodeSymMap;
    Diagnostics& _diagnostics;

    std::vector<Env> _envStack;

public:
    explicit NameResolveVisitor(SymbolTable& symTable, NodeSymbolMap& nodeSymMap, Diagnostics& diagnostics) noexcept
        : _symTable(symTable), _nodeSymMap(nodeSymMap), _diagnostics(diagnostics) { }

    void visit(Node* node) override;

    void visit(Name* name) override;

    void visit(LambdaExpr* lambda) override;

    void visit(VarDefStmt* vardef) override;
    void visit(FnDefStmt* fndef) override;
    void visit(TypeDefStmt* tdef) override;
    void visit(ModuleStmt* moddef) override;
    void visit(BlockExpr* block) override;

    void resolveBlock(BlockExpr* block, Env env);

private:
    /**
     * Pushes a new lexical environment.
     * @param env Environment to push (optional).
     */
    void pushEnv(Env env = {}) { _envStack.push_back(std::move(env)); }

    /**
     * Pop the current lexical environment.
     */
    void popEnv() { _envStack.pop_back(); }

    /**
     * Retrieves the current lexical environment.
     * @return The current lexical environment.
     */
    Env& currentEnv() { return _envStack.back(); }

    /**
     * Looks up a name starting from the current environment up to the global environment.
     * @param name Name to look up.
     * @return Symbol associated with the name or `nullptr` if no symbol was found, and whether the symbol is visible
     *         or not.
     */
    [[nodiscard]]
    std::pair<const Symbol*, bool> lookup(InternedNameValue name) const noexcept;

    /**
     * Adds a diagnostic to the result.
     * @param diagnostic Diagnostic to add.
     */
    void report(Diagnostic diagnostic) noexcept {
        _diagnostics.add(std::move(diagnostic));
    }

    friend class NameResolver;
};

} // Spark::FrontEnd
