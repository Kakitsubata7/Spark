#include "binder.hpp"

namespace Spark::FrontEnd {

void PatternBindingCollector::visit(BindingPattern* pattern) {
    _out.push_back(pattern->name);
}

void PatternBindingCollector::visit(TuplePattern* pattern) {
    for (Pattern* p : pattern->patterns) {
        p->accept(*this);
    }
}

void PatternBindingCollector::visit(CollectionPattern* pattern) {
    for (Pattern* p : pattern->prefix) {
        p->accept(*this);
    }
    for (Pattern* p : pattern->suffix) {
        p->accept(*this);
    }
}

void PatternBindingCollector::visit(RecordPattern* pattern) {
    for (RecordPatternField* f : pattern->fields) {
        f->pattern->accept(*this);
    }
}

void PatternBindingCollector::collect(Node* node, std::vector<Name*>& out) {
    if (node == nullptr) {
        return;
    }
    PatternBindingCollector v{out};
    node->accept(v);
}

std::vector<Name*> PatternBindingCollector::collect(Node* node) {
    std::vector<Name*> out;
    collect(node, out);
    return out;
}



void NameBinder::visit(Name* name) {
    // Ignore if node is already bound to a symbol
    if (_nodeSymMap.hasSymbol(name)) {
        return;
    }

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

void NameBinder::visit(BindingPattern* pattern) {
    for (Name* name : PatternBindingCollector::collect(pattern)) {
        name->accept(*this);
    }
}

void NameBinder::visit(TuplePattern* pattern) {
    for (Name* name : PatternBindingCollector::collect(pattern)) {
        name->accept(*this);
    }
}

void NameBinder::visit(CollectionPattern* pattern) {
    for (Name* name : PatternBindingCollector::collect(pattern)) {
        name->accept(*this);
    }
}

void NameBinder::visit(RecordPattern* pattern) {
    for (Name* name : PatternBindingCollector::collect(pattern)) {
        name->accept(*this);
    }
}

void NameBinder::bind(Node* node,
                      Env& env,
                      SymbolTable& symTable,
                      NodeSymbolMap& nodeSymMap,
                      SymbolKind kind,
                      bool isReassignable,
                      bool isReference,
                      bool isRedeclarable,
                      bool isVisible,
                      Diagnostics& diagnostics) {
    if (node == nullptr) {
        return;
    }
    NameBinder v{
        env, symTable, nodeSymMap, kind, isReassignable, isReference, isRedeclarable, isVisible, diagnostics
    };
    node->accept(v);
}



void DeclBinder::visit(VarDefStmt* vardef) {
    VarModifier* mod = vardef->mod;
    NameBinder v{
        _env, _symTable, _nodeSymMap, SymbolKind::Var, isReassignable(mod), isReference(mod),
        false, _isVisible, _diagnostics
    };
    vardef->pattern->accept(v);
}

void DeclBinder::visit(FnDefStmt* fndef) {
    NameBinder v{_env, _symTable, _nodeSymMap, SymbolKind::Func, false, false,
        true, _isVisible, _diagnostics
    };
    fndef->name->accept(v);
}

void DeclBinder::visit(TypeDefStmt* tdef) {
    NameBinder v{_env, _symTable, _nodeSymMap, SymbolKind::Type, false, false,
        false, _isVisible, _diagnostics
    };
    tdef->name->accept(v);
}

void DeclBinder::visit(ModuleStmt* moddef) {
    NameBinder v{_env, _symTable, _nodeSymMap, SymbolKind::Module, false, false,
        true, _isVisible, _diagnostics
    };
    moddef->path->segs[0]->name->accept(v);
}

void DeclBinder::bind(Node* node,
                      Env& env,
                      SymbolTable& symTable,
                      NodeSymbolMap& nodeSymMap,
                      bool isVisible,
                      Diagnostics& diagnostics) {
    if (node == nullptr) {
        return;
    }
    DeclBinder v{env, symTable, nodeSymMap, isVisible, diagnostics};
    node->accept(v);
}

} // Spark::FrontEnd
