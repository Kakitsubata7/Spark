#include "semantic_resolver.hpp"

#include <sstream>
#include <variant>

namespace Spark::FrontEnd {

void NameResolver::visit(Name* node) {
    assert(node != nullptr);

    std::string_view name = node->value.str();
    if (Symbol* symbol = currentEnv().find(name); symbol != nullptr) {
        // Map symbol to `Name` node
        _nodeSymbolMap.set(node, symbol);
    }
    cannotFindError(node->start, node->end, name);
}

void NameResolver::visit(FnParam* param) {
    assert(param != nullptr);

    // Declares the parameter name
    declare(param->pattern, currentEnv(), SymbolKind::Var, isReassignable(param->mod));

    // Resolves type and default value (if present)
    if (param->type != nullptr) {
        param->type->accept(*this);
    }
    if (param->def) {
        param->def->accept(*this);
    }
}

void NameResolver::visit(IfThenExpr* ifthen) {
    assert(ifthen != nullptr);
    ifthen->condition->accept(*this);
    ifthen->thenExpr->accept(*this);
    ifthen->elseExpr->accept(*this);
}

void NameResolver::visit(BlockExpr* block) {
    assert(block != nullptr);

    pushEnv();

    // Declare hoisted names
    for (Node* node : block->nodes) {
        if (isHoistedDeclarative(node)) {
            node->accept(*this);
        }
    }

    // Resolve the rest
    for (Node* node : block->nodes) {
        if (!isHoistedDeclarative(node)) {
            node->accept(*this);
        }
    }

    popEnv();
}

void NameResolver::visit(BinaryExpr* binary) {
    assert(binary != nullptr);
    binary->lhs->accept(*this);
    binary->rhs->accept(*this);
}

void NameResolver::visit(PrefixExpr* prefix) {
    assert(prefix != nullptr);
    prefix->expr->accept(*this);
}

void NameResolver::visit(PostfixExpr* postfix) {
    assert(postfix != nullptr);

    postfix->expr->accept(*this);
}

void NameResolver::visit(LiteralExpr* literal) {
    assert(literal != nullptr);
}

void NameResolver::resolve(LiteralExpr* literal, std::string& src) {
    assert(literal != nullptr);

    std::visit([&](auto&& value) {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, VoidLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("Void"));

            // Set C source
            src = _emitter.voidLiteral();
        } else if constexpr (std::is_same_v<T, IntLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("Int"));

            // Set C source
            src = _emitter.intLiteral(value.value);
        } else if constexpr (std::is_same_v<T, RealLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("Real"));

            // Set C source
            src = _emitter.realLiteral(value.value);
        } else if constexpr (std::is_same_v<T, BoolLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("Bool"));

            // Set C source
            src = _emitter.boolLiteral(value.value);
        } else if constexpr (std::is_same_v<T, StringLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("String"));

            // Set C source
            src = _emitter.stringLiteral(value.value);
        } else if constexpr (std::is_same_v<T, NilLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("Nil"));

            // Set C source
            src = _emitter.nilLiteral();
        }
    }, literal->literal);
}

void NameResolver::visit(NameExpr* ident) {
    assert(ident != nullptr);
    ident->name->accept(*this);
}

void NameResolver::visit(VarDefStmt* vardef) {
    assert(vardef != nullptr);
    declare(vardef->pattern, currentEnv(), SymbolKind::Var, isReassignable(vardef->mod));
}

void NameResolver::visit(FnDefStmt* fndef) {
    assert(fndef != nullptr);

    // Declares function name
    declare(fndef->name, currentEnv(), SymbolKind::Func, false);

    // Function body
    pushEnv();

    // Declares parameters
    for (FnParam* param : fndef->params) {
        param->accept(*this);
    }

    // Resolves body
    fndef->body->accept(*this);

    popEnv();
}

void NameResolver::visit(TypeDefStmt* tdef) {
    assert(tdef != nullptr);

    // Declares type name
    declare(tdef->name, currentEnv(), SymbolKind::Type, false);

    // Resolves base type list
    for (Expr* base : tdef->bases) {
        base->accept(*this);
    }

    // Resolves type body
    tdef->body->accept(*this);
}

void NameResolver::visit(IfStmt* ifstmt) {
    assert(ifstmt != nullptr);
    ifstmt->condition->accept(*this);
    ifstmt->thenBody->accept(*this);
    if (ifstmt->elseBody != nullptr) {
        ifstmt->elseBody->accept(*this);
    }
}

void NameResolver::visit(WhileStmt* w) {
    assert(w != nullptr);
    w->condition->accept(*this);
    w->body->accept(*this);
}

void NameResolver::visit(DoWhileStmt* dowhile) {
    assert(dowhile != nullptr);
    dowhile->body->accept(*this);
    dowhile->condition->accept(*this);
}

void NameResolver::visit(ForStmt* forstmt) {
    assert(forstmt != nullptr);

    // Resolves range
    forstmt->range->accept(*this);

    pushEnv();

    // Declares iterator
    declare(forstmt->iterator, currentEnv(), SymbolKind::Var, false);

    // Resolves body
    forstmt->body->accept(*this);

    popEnv();
}

void NameResolver::visit(ReturnStmt* ret) {
    assert(ret != nullptr);
    if (ret->expr != nullptr) {
        ret->expr->accept(*this);
    }
}

void NameResolver::declare(Name* node, Env& env, SymbolKind kind, bool isReassignable) {
    assert(node != nullptr);

    NameDeclarator declarator{_symbolTable, _nodeSymbolMap, env, kind, isReassignable, _diagnostics};
    node->accept(declarator);
}

void NameResolver::declare(Pattern* pattern, Env& env, SymbolKind kind, bool isReassignable) {
    assert(pattern != nullptr);

    NameDeclarator declarator{_symbolTable, _nodeSymbolMap, env, kind, isReassignable, _diagnostics};
    pattern->accept(declarator);
}

SemanticType* NameResolver::getGlobalDeclaredType(std::string_view name) const {
    return _globalEnv.get(name)->type;
}

bool NameResolver::isReassignable(VarModifier::VarKind kind) noexcept {
    return kind == VarModifier::VarKind::Let || kind == VarModifier::VarKind::Ref;
}

bool NameResolver::isReassignable(const VarModifier* varmod) noexcept {
    assert(varmod != nullptr);
    return isReassignable(varmod->kind);
}

bool NameResolver::isHoistedDeclarative(const Node* node) noexcept {
    assert(node != nullptr);

    if (node->is<FnDefStmt>() || node->is<TypeDefStmt>()) {
        return true;
    }
    return false;
}

void NameResolver::cannotFindError(Location start, Location end, std::string_view name) {
    std::ostringstream msg;
    msg << "cannot find name: `" << name << "`";
    _diagnostics.add(Diagnostic::error(start, end, msg.str()));
}



void NameDeclarator::visit(Name* node) {
    assert(node != nullptr);

    // Checks if the name is already declared locally in the environment
    std::string_view name = node->value.str();
    Symbol* symbol = _env.get(name);
    if (symbol == nullptr) {
        // Create symbol
        symbol = _symbolTable.make(Symbol{
            .kind = _kind,
            .isReassignable = _isReassignable,
            .node = node
        });

        // Bind symbol to environment
        _env.set(name, symbol);
    } else {
        // Redeclaration error, reuse the previously declared symbol
        redeclareError(node->start, node->end, name, symbol->start(), symbol->end());
    }

    // Bind symbol to the `Name` node
    _nodeSymbolMap.set(node, symbol);
}

void NameDeclarator::visit(BindingPattern* pattern) {
    assert(pattern != nullptr);
    pattern->name->accept(*this);
}

void NameDeclarator::visit(TuplePattern* pattern) {
    assert(pattern != nullptr);
    for (Pattern* p : pattern->patterns) {
        p->accept(*this);
    }
}

void NameDeclarator::visit(CollectionPattern* pattern) {
    assert(pattern != nullptr);
    for (Pattern* p : pattern->prefix) {
        p->accept(*this);
    }
    for (Pattern* p : pattern->suffix) {
        p->accept(*this);
    }
}

void NameDeclarator::visit(RecordPattern* pattern) {
    assert(pattern != nullptr);
    for (RecordPatternField* field : pattern->fields) {
        field->pattern->accept(*this);
    }
}

void NameDeclarator::redeclareError(Location start,
                                        Location end,
                                        std::string_view name,
                                        Location prevStart,
                                        Location prevEnd) noexcept {
    std::ostringstream msg;
    msg << "redeclaration of name: `" << name << "`";
    _diagnostics.add(Diagnostic::error(start, end, msg.str(), {
        Diagnostic::note(prevStart, prevEnd, "previously declared here")
    }));
}



void DeclaredTypeResolver::visit(NameExpr* ident) {
    assert(ident != nullptr);

    std::string_view name = ident->name->value.str();
    Symbol* symbol = _env.find(name);
}

void DeclaredTypeResolver::visit(BinaryExpr* binary) {

}

} // Spark::FrontEnd
