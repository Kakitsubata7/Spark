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

    bool _isReassignable;
    bool _isReference;
    bool _isVisible;

    Diagnostics& _diagnostics;

public:
    BindingVisitor(Env& env,
                   SymbolTable& symTable,
                   bool isReassignable,
                   bool isReference,
                   bool isVisible,
                   Diagnostics& diagnostics) noexcept
        : _env(env), _symTable(symTable), _isReassignable(isReassignable), _isReference(isReference),
          _isVisible(isVisible), _diagnostics(diagnostics) { }

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

} // Spark::FrontEnd
