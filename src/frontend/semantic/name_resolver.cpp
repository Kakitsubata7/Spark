#include "name_resolver.hpp"

#include <sstream>

namespace Spark::FrontEnd {

void PatternDeclarator::visit(BindingPattern* pattern) {
    assert(pattern != nullptr);

    Symbol* symbol = declare(pattern->name);
    _nodeSymbolMap.set(pattern, symbol); // Bind symbol to the `BindingPattern` node
}

void PatternDeclarator::visit(TuplePattern* pattern) {
    assert(pattern != nullptr);
    for (Pattern* p : pattern->patterns) {
        p->accept(*this);
    }
}

void PatternDeclarator::visit(CollectionPattern* pattern) {
    assert(pattern != nullptr);
    for (Pattern* p : pattern->prefix) {
        p->accept(*this);
    }
    for (Pattern* p : pattern->suffix) {
        p->accept(*this);
    }
}

void PatternDeclarator::visit(RecordPattern* pattern) {
    assert(pattern != nullptr);
    for (RecordPatternField* field : pattern->fields) {
        field->pattern->accept(*this);
    }
}

Symbol* PatternDeclarator::declare(Name* node) {
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

    return symbol;
}

void PatternDeclarator::redeclareError(Location start,
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

void NameDeclarator::visit(VarDefStmt* vardef) {
    assert(vardef != nullptr);
    bool isReassignable = vardef->mod->kind == VarModifier::VarKind::Let;
    PatternDeclarator declarator{_symbolTable, _nodeSymbolMap, _env, SymbolKind::Var, isReassignable, _diagnostics};
    vardef->pattern->accept(declarator);
}

void NameDeclarator::visit(FnDefStmt* fndef) {
    assert(fndef != nullptr);
    PatternDeclarator declarator{_symbolTable, _nodeSymbolMap, _env, SymbolKind::Func, false, _diagnostics};
    fndef->name->accept(declarator);
}

Symbol* NameResolver::resolve(const Name* node) {
    assert(node != nullptr);

    std::string_view name = node->value.str();
    if (Symbol* symbol = _env.find(name); symbol != nullptr) {
        // Map symbol to `Name` node
        _nodeSymbolMap.set(node, symbol);
        return symbol;
    }
    cannotFindError(node->start, node->end, name);
    return nullptr;
}

void NameResolver::visit(NameExpr* ident) {
    assert(ident != nullptr);

    if (Symbol* symbol = resolve(ident->name); symbol != nullptr) {
        _nodeSymbolMap.set(ident, symbol);
    }
}

void NameResolver::cannotFindError(Location start, Location end, std::string_view name) {
    std::ostringstream msg;
    msg << "cannot find name: `" << name << "`";
    _diagnostics.add(Diagnostic::error(start, end, msg.str()));
}



void SemanticResolver::visit(IfThenExpr* ifthen) {
    assert(ifthen != nullptr);

    ifthen->condition->accept(*this);
    ifthen->thenExpr->accept(*this);
    ifthen->elseExpr->accept(*this);
}

void SemanticResolver::visit(BlockExpr* block) {
    assert(block != nullptr);

    pushEnv();

    // Visit hoisted declarative nodes first
    for (Node* node : block->nodes) {
        if (isHoistedDeclarative(node)) {
            node->accept(*this);
        }
    }

    // Visit other nodes while ignoring hoisted declarative nodes
    for (Node* node : block->nodes) {
        if (!isHoistedDeclarative(node)) {
            node->accept(*this);
        }
    }

    popEnv();
}

void SemanticResolver::visit(BinaryExpr* binary) {
    assert(binary != nullptr);

    binary->lhs->accept(*this);
    binary->rhs->accept(*this);
}

void SemanticResolver::visit(PrefixExpr* prefix) {
    assert(prefix != nullptr);

    prefix->expr->accept(*this);
}

void SemanticResolver::visit(PostfixExpr* postfix) {
    assert(postfix != nullptr);

    postfix->expr->accept(*this);
}

void SemanticResolver::visit(NameExpr* ident) {
    assert(ident != nullptr);

    NameResolver nameResolver{_symbolTable, _nodeSymbolMap, currentEnv(), _diagnostics};
    ident->accept(nameResolver);
}

void SemanticResolver::visit(VarDefStmt* vardef) {
    assert(vardef != nullptr);

    NameDeclarator nameDeclarator{_symbolTable, _nodeSymbolMap, currentEnv(), _diagnostics};
    vardef->accept(nameDeclarator);
}

void SemanticResolver::visit(FnDefStmt* fndef) {
    assert(fndef != nullptr);

    NameDeclarator nameDeclarator{_symbolTable, _nodeSymbolMap, currentEnv(), _diagnostics};
    fndef->accept(nameDeclarator);
}

void SemanticResolver::visit(IfStmt* ifstmt) {
    assert(ifstmt != nullptr);

    ifstmt->condition->accept(*this);
    ifstmt->thenBody->accept(*this);
    if (ifstmt->elseBody != nullptr) {
        ifstmt->elseBody->accept(*this);
    }
}

void SemanticResolver::visit(WhileStmt* w) {
    assert(w != nullptr);

    w->condition->accept(*this);
    w->body->accept(*this);
}

void SemanticResolver::visit(DoWhileStmt* dowhile) {
    assert(dowhile != nullptr);

    dowhile->body->accept(*this);
    dowhile->condition->accept(*this);
}

void SemanticResolver::visit(ReturnStmt* ret) {
    assert(ret != nullptr);

    if (ret->expr != nullptr) {
        ret->expr->accept(*this);
    }
}

bool SemanticResolver::isHoistedDeclarative(const Node* node) noexcept {
    assert(node != nullptr);

    if (node->is<FnDefStmt>()) {
        return true;
    }
    return false;
}

} // Spark::FrontEnd
