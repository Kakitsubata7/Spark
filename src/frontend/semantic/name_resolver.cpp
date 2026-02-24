#include "name_resolver.hpp"

#include <sstream>

namespace Spark::FrontEnd {

static Diagnostic redeclareError(Location start, Location end, const Symbol* symbol) {
    std::ostringstream msg;
    msg << "redeclaration of `" << symbol->name() << "`";

    std::ostringstream note;
    note << "previously declared as a ";
    switch (symbol->kind) {
        case SymbolKind::Var:
            note << "variable";
            break;
        case SymbolKind::Func:
            note << "function";
            break;
        case SymbolKind::Type:
            note << "type";
            break;
        case SymbolKind::Module:
            note << "module";
            break;
    }
    note << " here";

    return Diagnostic::error(start, end, msg.str(), {
        Diagnostic::note(symbol->start(), symbol->end(), note.str())
    });
}

static Diagnostic cannotFindError(Location start,
                                  Location end,
                                  std::string_view name,
                                  const std::optional<std::string_view>& base) {
    std::ostringstream oss;
    oss << "cannot find name `" << name << "`";
    if (base.has_value()) {
        oss << " under `" << base.value() << "`";
    }
    return Diagnostic::error(start, end, oss.str());
}



void Declarator::declare(Name* name,
                         SymbolTable& symbolTable,
                         Env& env,
                         SymbolKind kind,
                         bool isReassignable,
                         Diagnostics& diagnostics) {
    Declarator declarator{symbolTable, env, kind, isReassignable, diagnostics};
    name->accept(declarator);
}

void Declarator::declare(Pattern* pattern,
                         SymbolTable& symbolTable,
                         Env& env,
                         SymbolKind kind,
                         bool isReassignable,
                         Diagnostics& diagnostics) {
    Declarator declarator{symbolTable, env, kind, isReassignable, diagnostics};
    pattern->accept(declarator);
}

void Declarator::visit(Name* name) {
    // Checks if the name already exists in the environment
    if (Symbol* symbol = _env.get(name->value.str()); symbol != nullptr) {
        diagnostic(redeclareError(name->start, name->end, symbol));
        return;
    }

    // Creates symbol and declares name
    _env.set(name->value.str(), _symbolTable.make(Symbol{
        .kind = _kind,
        .isReassignable = _isReassignable,
        .node = name
    }));
}

void Declarator::visit(BindingPattern* pattern) {
    pattern->name->accept(*this);
}

void Declarator::visit(TuplePattern* pattern) {
    for (Pattern* p : pattern->patterns) {
        p->accept(*this);
    }
}

void Declarator::visit(CollectionPattern* pattern) {
    for (Pattern* p : pattern->prefix) {
        p->accept(*this);
    }
    for (Pattern* p : pattern->suffix) {
        p->accept(*this);
    }
}

void Declarator::visit(RecordPattern* pattern) {
    for (RecordPatternField* field : pattern->fields) {
        field->pattern->accept(*this);
    }
}



void StmtDeclarator::declare(Stmt* stmt, NameResolveContext& ctx, Env& env, Diagnostics& diagnostics) {
    if (stmt == nullptr) {
        return;
    }

    StmtDeclarator declarator{ctx, env, diagnostics};
    stmt->accept(declarator);
}

void StmtDeclarator::visit(VarDefStmt* vardef) {
    Declarator::declare(vardef->pattern, _ctx.symbolTable(), _env, SymbolKind::Var,
        vardef->mod->kind == VarModifier::VarKind::Let, _diagnostics);
}

void StmtDeclarator::visit(FnDefStmt* fndef) {
    Declarator::declare(fndef->name, _ctx.symbolTable(), _env, SymbolKind::Func, false,
        _diagnostics);
}

void StmtDeclarator::visit(TypeDefStmt* tdef) {
    Declarator::declare(tdef->name, _ctx.symbolTable(), _env, SymbolKind::Type, false,
        _diagnostics);
}

void StmtDeclarator::visit(ModuleStmt* mdef) {
    std::vector<PathSeg*>& segs = mdef->path->segs;

    Env* currentEnv = &_env;

    for (PathSeg* seg : segs) {
        Name* name = seg->name;
        Symbol* symbol = currentEnv->get(name->value.str());

        if (symbol != nullptr) {
            // Checks if the name was declared with a different kind
            if (symbol->kind != SymbolKind::Module) {
                diagnostic(redeclareError(name->start, name->end, symbol));
                return;
            }
        } else {
            // Creates a new symbol and bind it under the current env
            symbol = _ctx.makeSymbol(Symbol{
                .kind = SymbolKind::Module,
                .isReassignable = false,
                .node = name
            });
            currentEnv->set(name->value.str(), symbol);
        }

        currentEnv = &_ctx.getOrCreateSubEnv(symbol);
    }

    // Declares exported names in the module to the last symbol in the path
    for (Node* node : mdef->body->nodes) {
        if (auto* e = node->as<ExportStmt>()) {
            declare(e->stmt, _ctx, *currentEnv, _diagnostics);
        }
    }
}



void GlobalNameResolver::resolve(const AST& ast, NameResolveContext& ctx, Diagnostics& diagnostics) {
    if (ast.root == nullptr) {
        return;
    }

    GlobalNameResolver resolver{ctx, diagnostics};
    if (auto* block = ast.root->as<BlockExpr>()) {
        for (Node* node : block->nodes) {
            node->accept(resolver);
        }
    } else {
        ast.root->accept(resolver);
    }
}

void GlobalNameResolver::visit(ExportStmt* e) {
    StmtDeclarator::declare(e->stmt, _ctx, _ctx.globalEnv(), _diagnostics);
}



void NameResolver::resolve(const AST& ast, NameResolveContext& ctx, Diagnostics& diagnostics) {
    if (ast.root == nullptr) {
        return;
    }

    NameResolver resolver{ctx, diagnostics};
    ast.root->accept(resolver);
}

void NameResolver::visit(LambdaExpr* expr) {

}

void NameResolver::visit(IfThenExpr* expr) {
    expr->condition->accept(*this);
    expr->thenExpr->accept(*this);
    expr->elseExpr->accept(*this);
}

void NameResolver::visit(TryElseExpr* expr) {
    expr->tryExpr->accept(*this);
    expr->elseExpr->accept(*this);
}

void NameResolver::visit(MatchExpr* expr) {
    // Resolves scrutinee
    expr->scrutinee->accept(*this);

    for (MatchCase* c : expr->cases) {
        Env& env = pushEnv();

        // Declares case binding pattern
        if (c->pattern != nullptr) {
            Declarator::declare(c->pattern, _ctx.symbolTable(), env, SymbolKind::Var, false,
                _diagnostics);
        }

        // Resolves guard
        if (c->guard != nullptr) {
            c->guard->accept(*this);
        }

        // Resolves body
        pushEnv();
        c->body->accept(*this);
        popEnv();

        popEnv();
    }
}

void NameResolver::visit(TryCatchExpr* expr) {
    // Resolves try body
    expr->expr->accept(*this);

    // Resolve catch clauses
    for (CatchClause* c : expr->catches) {
        pushEnv();
        c->body->accept(*this);
        popEnv();
    }
}

void NameResolver::visit(ThrowExpr* expr) {
    expr->expr->accept(*this);
}

void NameResolver::visit(BlockExpr* expr) {
    pushEnv();
    for (Node* node : expr->nodes) {
        node->accept(*this);
    }
    popEnv();
}

void NameResolver::visit(IsExpr* expr) {
    expr->expr->accept(*this);
    expr->type->accept(*this);
}

void NameResolver::visit(AsExpr* expr) {
    // expr->expr->accept(*this);
    // Declarator::declare(expr->pattern, _ctx.symbolTable(), currentEnv(), SymbolKind::Var, false,
    //     _diagnostics);
    // expr->type->accept(*this);
    std::cerr << "`as` is not supported" << std::endl;
    abort();
}

void NameResolver::visit(BinaryExpr* expr) {
    expr->lhs->accept(*this);
    expr->rhs->accept(*this);
}

void NameResolver::visit(PrefixExpr* expr) {
    expr->expr->accept(*this);
}

void NameResolver::visit(PostfixExpr* expr) {
    expr->expr->accept(*this);
}

void NameResolver::visit(CallExpr* expr) {
    expr->callee->accept(*this);
    for (CallArg* arg : expr->args) {
        arg->expr->accept(*this);
    }
}

void NameResolver::visit(SubscriptExpr* expr) {
    expr->base->accept(*this);
    for (Expr* index : expr->indices) {
        index->accept(*this);
    }
}

void NameResolver::visit(NameExpr* expr) {
    resolveName(expr->name, currentEnv());
}

void NameResolver::visit(GlobalAccessExpr* expr) {
    resolveName(expr->name, _ctx.globalEnv(), "global");
}

void NameResolver::visit(UpvalueExpr* expr) {
    std::cerr << "`$` is not supported" << std::endl;
    abort();
}

void NameResolver::visit(MemberAccessExpr* expr) {
    // Resolves base expression
    expr->base->accept(*this);

    // TODO
}

void NameResolver::visit(TupleExpr* expr) {
    for (Expr* e : expr->exprs) {
        e->accept(*this);
    }
}

void NameResolver::visit(CollectionExpr* expr) {
    for (Expr* e : expr->exprs) {
        e->accept(*this);
    }
}

void NameResolver::visit(TypeofExpr* expr) {
    expr->expr->accept(*this);
}

void NameResolver::visit(VarDefStmt* stmt) {

}

void NameResolver::visit(FnDefStmt* stmt) {

}

void NameResolver::visit(TypeDefStmt* stmt) {

}

void NameResolver::visit(CaseDefStmt* stmt) {
    std::cerr << "enum member and ADT member are not supported" << std::endl;
    abort();
}

void NameResolver::visit(AssignStmt* stmt) {
    stmt->lhs->accept(*this);
    stmt->rhs->accept(*this);
}

void NameResolver::visit(IfStmt* stmt) {
    stmt->condition->accept(*this);
    stmt->thenBody->accept(*this);
    if (stmt->elseBody != nullptr) {
        stmt->elseBody->accept(*this);
    }
}

void NameResolver::visit(WhileStmt* stmt) {
    stmt->condition->accept(*this);
    stmt->body->accept(*this);
}

void NameResolver::visit(DoWhileStmt* stmt) {
    stmt->body->accept(*this);
    stmt->condition->accept(*this);
}

void NameResolver::visit(ForStmt* stmt) {
    // Resolves range
    stmt->range->accept(*this);

    Env& forEnv = pushEnv();

    // Declares iterator
    Declarator::declare(stmt->iterator, _ctx.symbolTable(), forEnv, SymbolKind::Var, false,
        _diagnostics);

    // Resolves body
    stmt->body->accept(*this);

    popEnv();
}

void NameResolver::visit(ReturnStmt* stmt) {
    if (stmt->expr != nullptr) {
        stmt->expr->accept(*this);
    }
}

void NameResolver::visit(ModuleStmt* stmt) {
    StmtDeclarator::declare(stmt, _ctx, currentEnv(), _diagnostics);
    // TODO
}

void NameResolver::visit(ExportStmt* stmt) {

}

void NameResolver::visit(UndefineStmt* stmt) {
    Env* env = &currentEnv();
    std::optional<std::string_view> base = std::nullopt;
    for (PathSeg* seg : stmt->path->segs) {
        // Check if previous name has a sub-environment, if not, it means the symbol cannot be found under that name
        if (env == nullptr) {
            diagnostic(cannotFindError(seg->name->start, seg->name->end, seg->name->value.str(), base));
            return;
        }

        Symbol* symbol = resolveName(seg->name, *env, base);
        env = (symbol != nullptr && _ctx.hasSubEnv(symbol)) ? &_ctx.getSubEnv(symbol) : nullptr;
        base = seg->name->value.str();
    }
}

Symbol* NameResolver::resolveName(const Name* name, const Env& env, const std::optional<std::string_view>& base) {
    Symbol* symbol = env.get(name->value.str());
    if (symbol == nullptr) {
        diagnostic(cannotFindError(name->start, name->end, name->value.str(), base));
        return nullptr;
    }
    return symbol;
}
} // Spark::FrontEnd
