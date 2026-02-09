#include "binding_visitor.hpp"

namespace Spark::FrontEnd {

void BindingVisitor::visit(Name* name) {
    // Check if name is already declared in the same env
    if (Symbol* s = _env.find(name->value); s != nullptr) {
        report(Diagnostic::redeclareError(name->start, name->end, s));
        return;
    }

    // Define symbol from name
    Symbol* s = _symTable.make(Symbol{
        .node = name,
        .isReassignable = _isReassignable,
        .isReference = _isReference
    });
    _env.declare(s, _isVisible);
}

void BindingVisitor::visit(BindingPattern* pattern) {
    pattern->name->accept(*this);
}

void BindingVisitor::visit(TuplePattern* pattern) {
    for (Pattern* cp : pattern->patterns) {
        cp->accept(*this);
    }
}

void BindingVisitor::visit(CollectionPattern* pattern) {
    for (Pattern* cp : pattern->prefix) {
        cp->accept(*this);
    }
    for (Pattern* cp : pattern->suffix) {
        cp->accept(*this);
    }
}

void BindingVisitor::visit(RecordPattern* pattern) {
    for (RecordPatternField* field : pattern->fields) {
        field->pattern->accept(*this);
    }
}

} // Spark::FrontEnd
