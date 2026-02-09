#pragma once

#include "env.hpp"
#include "symbol.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a visitor that traverses a node and binds name(s) to symbol(s) to an environment.
 */
class BindingVisitor : public NodeVisitor {
private:
    Env& _env;
    SymbolTable& _symTable;
    NodeSymbolMap& _nodeSymMap;

    bool _isReassignable;
    bool _isReference;
    bool _isRedeclarable;
    bool _isVisible;

    Diagnostics& _diagnostics;

public:
    BindingVisitor(Env& env,
                   SymbolTable& symTable,
                   NodeSymbolMap& nodeSymMap,
                   bool isReassignable,
                   bool isReference,
                   bool isRedeclarable,
                   bool isVisible,
                   Diagnostics& diagnostics) noexcept
        : _env(env), _symTable(symTable), _nodeSymMap(nodeSymMap), _isReassignable(isReassignable),
          _isReference(isReference), _isRedeclarable(isRedeclarable), _isVisible(isVisible),
          _diagnostics(diagnostics) { }

    void visit(Name* name) override;
    void visit(BindingPattern* pattern) override;
    void visit(TuplePattern* pattern) override;
    void visit(CollectionPattern* pattern) override;
    void visit(RecordPattern* pattern) override;

private:
    /**
     * Adds a diagnostic to the result.
     * @param diagnostic Diagnostic to add.
     */
    void report(Diagnostic diagnostic) noexcept {
        _diagnostics.add(std::move(diagnostic));
    }
};

/**
 * Represents a wrapper to `BindingVisitor` that binds a definition node's names to the given environment.
 */
class Binder {
private:
    class BinderVisitor : public NodeVisitor {
    private:
        Env& _env;
        SymbolTable& _symTable;
        NodeSymbolMap& _nodeSymMap;
        bool _isVisible;
        Diagnostics& _diagnostics;

    public:
        BinderVisitor(Env& env, SymbolTable& symTable, NodeSymbolMap& nodeSymMap, bool isVisible,
                      Diagnostics& diagnostics) noexcept
            : _env(env), _symTable(symTable), _nodeSymMap(nodeSymMap), _isVisible(isVisible),
              _diagnostics(diagnostics) { }

        void visit(VarDefStmt* vardef) override;
        void visit(FnDefStmt* fndef) override;
        void visit(TypeDefStmt* tdef) override;
        void visit(ModuleStmt* moddef) override;

    private:
        static bool isReassignable(const VarModifier* mod) noexcept {
            VarModifier::VarKind kind = mod->kind;
            return kind == VarModifier::VarKind::Let || kind == VarModifier::VarKind::Ref;
        }

        static bool isReference(const VarModifier* mod) noexcept {
            VarModifier::VarKind kind = mod->kind;
            return kind == VarModifier::VarKind::Ref || kind == VarModifier::VarKind::Cref;
        }
    };

public:
    /**
     * Binds names of a definition node to the current environment, creating symbols.
     * @param node Definition node.
     * @param env Environment to bind names to.
     * @param symTable Symbol table.
     * @param nodeSymMap AST node symbol map.
     * @param isVisible Whether the bound symbols are visible in the environment or not.
     * @param diagnostics Diagnostics go in here.
     */
    static void bind(Node* node,
                     Env& env,
                     SymbolTable& symTable,
                     NodeSymbolMap& nodeSymMap,
                     bool isVisible,
                     Diagnostics& diagnostics);
};

} // Spark::FrontEnd
