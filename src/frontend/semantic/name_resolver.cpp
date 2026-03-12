#include "name_resolver.hpp"

#include <sstream>

namespace Spark::FrontEnd {

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

    NameDeclarator declarator{_symbolTable, _nodeSymbolMap, currentEnv(), SymbolKind::Var, };
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

void NameResolver::visit(NameExpr* ident) {
    assert(ident != nullptr);
    ident->name->accept(*this);
}

void NameResolver::visit(VarDefStmt* vardef) {
    assert(vardef != nullptr);
    bool isReassignable = vardef->mod->kind == VarModifier::VarKind::Let;
    declare(vardef->pattern, currentEnv(), SymbolKind::Var, isReassignable);
}

void NameResolver::visit(FnDefStmt* fndef) {
    assert(fndef != nullptr);

    // Declares function name
    declare(fndef->name, currentEnv(), SymbolKind::Func, false);

    //
    pushEnv();

    for (FnParam* param : fndef->params) {
        param->
    }

    fndef->body->accept(*this);

    popEnv();
}

void NameResolver::visit(TypeDefStmt* tdef) {
    assert(tdef != nullptr);
    declare(tdef->name, currentEnv(), SymbolKind::Type, false);
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

bool NameResolver::isReassignable(const VarModifier* varmod) noexcept {
    assert(varmod != nullptr);
    return varmod->kind == VarModifier::VarKind::Let || varmod->kind == VarModifier::VarKind::Ref;
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

} // Spark::FrontEnd
