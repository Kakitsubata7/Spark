#include "name_resolver.hpp"

#include <sstream>
#include <string_view>
#include <vector>

namespace Spark::FrontEnd {

static Diagnostic redeclError(const std::optional<std::string>& filename,
                              Location start,
                              Location end,
                              std::string_view name) {
    std::ostringstream oss;
    oss << "redeclaration of `" << name << "`";
    return Diagnostic::error(filename, start, end, oss.str());
}

static Diagnostic cannotFindError(const std::optional<std::string>& filename,
                                  Location start,
                                  Location end,
                                  std::string_view name) {
    std::ostringstream oss;
    oss << "cannot find symbol `" << name << "`";
    return Diagnostic::error(filename, start, end, oss.str());
}

void NameResolver::ResolveVisitor::visit(Node& node) {
    for (Node* child : node.getChildren()) {
        child->accept(*this);
    }
}

void NameResolver::ResolveVisitor::visit(Name& name) {
    if (lookup(name.name) == nullptr) {
        diagnostic(cannotFindError(_filename, name.start, name.end, name.name.str()));
    }
}

void NameResolver::ResolveVisitor::visit(VarDefStmt& vardef) {
    PatternBinder binder{currentEnv(), _symTable, isReassignable(vardef.mod), isReference(vardef.mod)};
    vardef.pattern->accept(binder);

    // Append diagnostics from binder
    result.diagnostics.insert(result.diagnostics.end(), binder.diagnostics.begin(),
        binder.diagnostics.end());
}

void NameResolver::ResolveVisitor::visit(FnDefStmt& fndef) {
    Env& env = currentEnv();

    // Check if name is already declared in the same env
    if (Symbol* s = env.get(fndef.name->name); s != nullptr) {
        diagnostic(redeclError(_filename, fndef.name->start, fndef.name->end, fndef.name->name.str()));
        return;
    }

    // Define symbol from name
    Symbol* s = _symTable.make(Symbol{
        .name = fndef.name->name,
        .defStart = fndef.name->start,
        .defEnd = fndef.name->end,
        .isReassignable = false,
        .isReference = false
    });
    _symTable.set(fndef.name, s);
    env.set(fndef.name->name, s);
}

void NameResolver::ResolveVisitor::visit(TypeDefStmt& tdef) {
    Env& env = currentEnv();

    // Check if name is already declared in the same env
    if (Symbol* s = env.get(tdef.name->name); s != nullptr) {
        diagnostic(redeclError(_filename, tdef.name->start, tdef.name->end, tdef.name->name.str()));
        return;
    }

    // Define symbol from name
    Symbol* s = _symTable.make(Symbol{
        .name = tdef.name->name,
        .defStart = tdef.name->start,
        .defEnd = tdef.name->end,
        .isReassignable = false,
        .isReference = false
    });
    _symTable.set(tdef.name, s);
    env.set(tdef.name->name, s);
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
    if (Symbol* s = _env.get(p.name->name); s != nullptr) {
        diagnostic(redeclError(_filename, p.name->start, p.name->end, p.name->name.str()));
        return;
    }

    // Define symbol from name
    Symbol* s = _symTable.make(Symbol{
        .name = p.name->name,
        .defStart = p.name->start,
        .defEnd = p.name->end,
        .isReassignable = _isReassignable,
        .isReference = _isReference
    });
    _symTable.set(p.name, s);
    _env.set(p.name->name, s);
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
