#include "structure_checker.hpp"

namespace Spark::FrontEnd {

void DeclarativePatternChecker::check(Node* node, Diagnostics& diagnostics) {
    DeclarativePatternChecker checker{diagnostics};
    node->accept(checker);
}

void DeclarativePatternChecker::visit(LiteralPattern* pattern) {
    _diagnostics.add(Diagnostic::error(pattern->start, pattern->end, "literal pattern is not allowed inside a declarative context"));
}

void DeclarativePatternChecker::visit(BindingPattern* pattern) { }

void DeclarativePatternChecker::visit(TuplePattern* pattern) {
    for (Pattern* p : pattern->patterns) {
        p->accept(*this);
    }
}

void DeclarativePatternChecker::visit(CollectionPattern* pattern) {
    for (Pattern* p : pattern->prefix) {
        p->accept(*this);
    }
    for (Pattern* p : pattern->suffix) {
        p->accept(*this);
    }
}

void DeclarativePatternChecker::visit(RecordPattern* pattern) {
    for (RecordPatternField* field : pattern->fields) {
        field->pattern->accept(*this);
    }
}



void StructureChecker::check(const AST& ast, Diagnostics& diagnostics) {
    if (ast.root == nullptr) {
        return;
    }

    StructureChecker checker{diagnostics};
    checker.replaceCtx(Context{
        .inLoop = false,
        .inFunc = false,
        .inType = false,
        .inModule = true
    });
    ast.root->accept(checker);
}

void StructureChecker::visit(Expr* expr) {
    // Fall back for non-declarative expressions
    if (inType()) {
        error(expr->start, expr->end, "non-declarative expression is not allowed inside a type declaration body");
    }
}

void StructureChecker::visit(Stmt* stmt) {
    // Fall back for non-declarative statements
    if (inType()) {
        error(stmt->start, stmt->end, "non-declarative statement is not allowed inside a type declaration body");
    }
}

void StructureChecker::visit(BlockExpr* block) {
    // Represents a standalone block expression, does not represent a statement's body
    Context old = replaceCtx(Context{
        .inLoop = inLoop(),
        .inFunc = inFunc(),
        .inType = false,
        .inModule = false
    });

    for (Node* node : block->nodes) {
        node->accept(*this);
    }

    replaceCtx(old);
}

void StructureChecker::visit(MatchExpr* match) {
    for (MatchCase* c : match->cases) {
        c->accept(*this);
    }
}

void StructureChecker::visit(MatchCase* c) {
    DeclarativePatternChecker::check(c, _diagnostics);
}

void StructureChecker::visit(VarDefStmt* vardef) {
    DeclarativePatternChecker::check(vardef->pattern, _diagnostics);
}

void StructureChecker::visit(WhileStmt* w) {
    visitLoop(w->body);
}

void StructureChecker::visit(ForStmt* f) {
    visitLoop(f->body);
}

void StructureChecker::visit(FnDefStmt* fndef) {
    visitFn(fndef->body);
}

void StructureChecker::visit(LambdaExpr* lambda) {
    visitFn(lambda->body);
}

void StructureChecker::visit(TypeDefStmt* tdef) {
    Context old = replaceCtx(Context{
        .inLoop = false,
        .inFunc = false,
        .inType = true,
        .inModule = false
    });

    for (Node* node : tdef->body->nodes) {
        node->accept(*this);
    }

    replaceCtx(old);
}

void StructureChecker::visit(ModuleStmt* moddef) {
    if (inLoop() || inFunc() || inType()) {
        error(moddef->start, moddef->end, "module declaration is only allowed inside a module");
    }

    Context old = replaceCtx(Context{
        .inLoop = false,
        .inFunc = false,
        .inType = false,
        .inModule = true
    });

    for (Node* node : moddef->body->nodes) {
        node->accept(*this);
    }

    replaceCtx(old);
}

void StructureChecker::visit(BreakStmt* b) {
    if (!inLoop()) {
        error(b->start, b->end, "`break` is only allowed inside a loop");
    }
}

void StructureChecker::visit(ContinueStmt* c) {
    if (!inLoop()) {
        error(c->start, c->end, "`continue` is only allowed inside a loop");
    }
}

void StructureChecker::visit(ReturnStmt* r) {
    if (!inFunc()) {
        error(r->start, r->end, "`return` is only allowed inside a function");
    }
}

void StructureChecker::visit(ExportStmt* e) {
    if (!inModule()) {
        error(e->start, e->end, "`export` is only allowed inside a module");
    }

    if (!e->stmt->is<VarDefStmt>() && !e->stmt->is<FnDefStmt>() && !e->stmt->is<TypeDefStmt>() && !e->stmt->is<ModuleStmt>()) {
        error(e->start, e->end, "cannot export non-declaration statement");
    }
}

void StructureChecker::visit(UndefineStmt* undef) {
    if (!inModule()) {
        error(undef->start, undef->end, "`undefine` is only allowed inside a module");
    }
}

void StructureChecker::visitLoop(BlockExpr* body) {
    Context old = replaceCtx(Context{
        .inLoop = true,
        .inFunc = inFunc(),
        .inType = false,
        .inModule = false
    });

    for (Node* node : body->nodes) {
        node->accept(*this);
    }

    replaceCtx(old);
}

void StructureChecker::visitFn(Expr* body) {
    Context old = replaceCtx(Context{
        .inLoop = false,
        .inFunc = true,
        .inType = false,
        .inModule = false
    });

    if (auto* block = body->as<BlockExpr>()) {
        for (Node* node : block->nodes) {
            node->accept(*this);
        }
    } else {
        body->accept(*this);
    }

    replaceCtx(old);
}

void StructureChecker::visitDeclPattern(Pattern* pattern) {
    if (pattern->as<CollectionPattern>()) {

    }
}

} // Spark::FrontEnd
