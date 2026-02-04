#include "name_resolver.hpp"

#include <sstream>
#include <string_view>
#include <vector>

namespace Spark::FrontEnd {

static Diagnostic redeclareError(Location start,
                                 Location end,
                                 std::string_view name) {
    std::ostringstream oss;
    oss << "redeclaration of `" << name << "`";
    return Diagnostic::error(start, end, oss.str());
}

static Diagnostic cannotFindError(Location start,
                                  Location end,
                                  std::string_view name) {
    std::ostringstream oss;
    oss << "cannot find symbol `" << name << "`";
    return Diagnostic::error(start, end, oss.str());
}

void NameResolver::ResolveVisitor::visit(Node& node) {
    for (Node* child : node.getChildren()) {
        child->accept(*this);
    }
}

void NameResolver::ResolveVisitor::visit(Name& name) {
    Symbol* s = lookup(name.value);
    if (s == nullptr) {
        report(cannotFindError(name.start, name.end, name.value.str()));
        return;
    }
    _symTable.set(&name, s);
}

void NameResolver::ResolveVisitor::visit(VarDefStmt& vardef) {
    // Resolve pattern
    PatternBinder binder{currentEnv(), _symTable, isReassignable(vardef.mod), isReference(vardef.mod)};
    vardef.pattern->accept(binder);
    result.diagnostics.adopt(binder.diagnostics);

    // Resolve type
    if (vardef.type != nullptr) {
        vardef.type->accept(*this);
    }

    // Resolve rhs
    if (vardef.rhs != nullptr) {
        vardef.rhs->accept(*this);
    }
}

void NameResolver::ResolveVisitor::visit(FnDefStmt& fndef) {
    Env& env = currentEnv();

    // Check if name is already declared in the same env
    if (Symbol* s = env.get(fndef.name->value); s != nullptr) {
        report(redeclareError(fndef.name->start, fndef.name->end, fndef.name->value.str()));
        return;
    }

    // Define symbol from name
    Symbol* s = _symTable.make(Symbol{
        .name = fndef.name->value,
        .defStart = fndef.name->start,
        .defEnd = fndef.name->end,
        .isReassignable = false,
        .isReference = false
    });
    _symTable.set(fndef.name, s);
    env.set(fndef.name->value, s);
}

void NameResolver::ResolveVisitor::visit(TypeDefStmt& tdef) {
    Env& env = currentEnv();

    // Check if name is already declared in the same env
    if (Symbol* s = env.get(tdef.name->value); s != nullptr) {
        report(redeclareError(tdef.name->start, tdef.name->end, tdef.name->value.str()));
        return;
    }

    // Define symbol from name
    Symbol* s = _symTable.make(Symbol{
        .name = tdef.name->value,
        .defStart = tdef.name->start,
        .defEnd = tdef.name->end,
        .isReassignable = false,
        .isReference = false
    });
    _symTable.set(tdef.name, s);
    env.set(tdef.name->value, s);
}

void NameResolver::ResolveVisitor::visit(BlockExpr& block) {
    pushEnv();

    // Define hoisted symbols (functions, types and modules)
    for (Node* child : block.nodes) {
        if (isHoisted(child)) {
            child->accept(*this);
        }
    }

    // Resolve others
    for (Node* child : block.nodes) {
        if (!isHoisted(child)) {
            child->accept(*this);
        }
    }

    popEnv();
}

Symbol* NameResolver::ResolveVisitor::lookup(InternedNameValue name) const noexcept {
    for (auto it = _envStack.rbegin(); it != _envStack.rend(); ++it) {
        const Env& env = *it;
        if (Symbol* s = env.get(name); s != nullptr) {
            return s;
        }
    }
    return nullptr;
}

void NameResolver::PatternBinder::visit(BindingPattern& p) {
    // Check if name is already declared in the same env
    if (Symbol* s = _env.get(p.name->value); s != nullptr) {
        report(redeclareError(p.name->start, p.name->end, p.name->value.str()));
        return;
    }

    // Define symbol from name
    Symbol* s = _symTable.make(Symbol{
        .name = p.name->value,
        .defStart = p.name->start,
        .defEnd = p.name->end,
        .isReassignable = _isReassignable,
        .isReference = _isReference
    });
    _symTable.set(p.name, s);
    _env.set(p.name->value, s);
}

void NameResolver::PatternBinder::visit(TuplePattern& p) {
    for (Pattern* cp : p.patterns) {
        cp->accept(*this);
    }
}

void NameResolver::PatternBinder::visit(CollectionPattern& p) {
    for (Pattern* cp : p.prefix) {
        cp->accept(*this);
    }
    for (Pattern* cp : p.suffix) {
        cp->accept(*this);
    }
}

void NameResolver::PatternBinder::visit(RecordPattern& p) {
    for (RecordPatternField* field : p.fields) {
        field->pattern->accept(*this);
    }
}

NameResolveResult NameResolver::resolve(const AST& ast, SymbolTable& symTable) {
    if (ast.root == nullptr) {
        return {};
    }

    ResolveVisitor visitor(symTable);
    ast.root->accept(visitor);
    return std::move(visitor.result);
}

} // Spark::FrontEnd
