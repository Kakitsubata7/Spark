#pragma once

#include "env.hpp"
#include "symbol.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a visitor that collects binding names from patterns.
 */
class PatternBindingCollector : public NodeVisitor {
private:
    std::vector<Name*>& _out;

public:
    explicit PatternBindingCollector(std::vector<Name*>& out) noexcept : _out(out) { }

    void visit(BindingPattern* pattern) override;
    void visit(TuplePattern* pattern) override;
    void visit(CollectionPattern* pattern) override;
    void visit(RecordPattern* pattern) override;

    static void collect(Node* node, std::vector<Name*>& out);
    static std::vector<Name*> collect(Node* node);
};

/**
 * Represents a visitor that traverses a node and binds name(s) to symbol(s) to an environment.
 */
class NameBinder : public NodeVisitor {
private:
    Env& _env;
    SymbolTable& _symTable;
    NodeSymbolMap& _nodeSymMap;

    SymbolKind _kind;
    bool _isReassignable;
    bool _isReference;
    bool _isRedeclarable;
    bool _isVisible;

    Diagnostics& _diagnostics;

public:
    NameBinder(Env& env,
               SymbolTable& symTable,
               NodeSymbolMap& nodeSymMap,
               SymbolKind kind,
               bool isReassignable,
               bool isReference,
               bool isRedeclarable,
               bool isVisible,
               Diagnostics& diagnostics) noexcept
        : _env(env), _symTable(symTable), _nodeSymMap(nodeSymMap), _kind(kind), _isReassignable(isReassignable),
          _isReference(isReference), _isRedeclarable(isRedeclarable), _isVisible(isVisible),
          _diagnostics(diagnostics) { }

    void visit(Name* name) override;
    void visit(BindingPattern* pattern) override;
    void visit(TuplePattern* pattern) override;
    void visit(CollectionPattern* pattern) override;
    void visit(RecordPattern* pattern) override;

    static void bind(Node* node,
                     Env& env,
                     SymbolTable& symTable,
                     NodeSymbolMap& nodeSymMap,
                     SymbolKind kind,
                     bool isReassignable,
                     bool isReference,
                     bool isRedeclarable,
                     bool isVisible,
                     Diagnostics& diagnostics);

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
 * Represents a visitor that binds names of a declaration node.
 */
class DeclBinder : public NodeVisitor {
private:
    Env& _env;
    SymbolTable& _symTable;
    NodeSymbolMap& _nodeSymMap;
    bool _isVisible;
    Diagnostics& _diagnostics;

public:
    DeclBinder(Env& env,
               SymbolTable& symTable,
               NodeSymbolMap& nodeSymMap,
               bool isVisible,
               Diagnostics& diagnostics) noexcept
        : _env(env), _symTable(symTable), _nodeSymMap(nodeSymMap), _isVisible(isVisible), _diagnostics(diagnostics) { }

    void visit(FnParam* param) override;

    void visit(VarDefStmt* vardef) override;
    void visit(FnDefStmt* fndef) override;
    void visit(TypeDefStmt* tdef) override;
    void visit(ModuleStmt* moddef) override;

    static void bind(Node* node,
                     Env& env,
                     SymbolTable& symTable,
                     NodeSymbolMap& nodeSymMap,
                     bool isVisible,
                     Diagnostics& diagnostics);
};

} // Spark::FrontEnd
