#include "binder.hpp"

namespace Spark::FrontEnd {

void BindingVisitor::visit(Name* name) {
    // Check if name is already declared in the same env
    if (const Symbol* s = _env.find(name->value); s != nullptr) {
        if (_isRedeclarable) {
            if (s->kind != _kind) {
                report(Diagnostic::conflictingRedeclareError(name->start, name->end, s));
                return;
            }
            _nodeSymMap.set(name, s);
        } else {
            report(Diagnostic::redeclareError(name->start, name->end, s));
        }
        return;
    }

    // Define symbol from name
    const Symbol* s = _symTable.make(Symbol{
        .node = name,
        .kind = _kind,
        .isReassignable = _isReassignable,
        .isReference = _isReference
    });
    _env.declare(s, _isVisible);
    _nodeSymMap.set(name, s);
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



void Binder::BinderVisitor::visit(VarDefStmt* vardef) {
    VarModifier* mod = vardef->mod;
    BindingVisitor v{
        _env, _symTable, _nodeSymMap, SymbolKind::Var, isReassignable(mod), isReference(mod),
        false, _isVisible, _diagnostics
    };
    vardef->pattern->accept(v);
}

void Binder::BinderVisitor::visit(FnDefStmt* fndef) {
    BindingVisitor v{_env, _symTable, _nodeSymMap, SymbolKind::Func, false, false,
        true, _isVisible, _diagnostics
    };
    fndef->name->accept(v);
}

void Binder::BinderVisitor::visit(TypeDefStmt* tdef) {
    BindingVisitor v{_env, _symTable, _nodeSymMap, SymbolKind::Type, false, false,
        false, _isVisible, _diagnostics
    };
    tdef->name->accept(v);
}

void Binder::BinderVisitor::visit(ModuleStmt* moddef) {
    BindingVisitor v{_env, _symTable, _nodeSymMap, SymbolKind::Module, false, false,
        true, _isVisible, _diagnostics
    };
    moddef->path->segs[0]->name->accept(v);
}

void Binder::bind(Node* node,
                  Env& env,
                  SymbolTable& symTable,
                  NodeSymbolMap& nodeSymMap,
                  bool isVisible,
                  Diagnostics& diagnostics) {
    if (node == nullptr) {
        return;
    }
    BinderVisitor v{env, symTable, nodeSymMap, isVisible, diagnostics};
    node->accept(v);
}

} // Spark::FrontEnd
