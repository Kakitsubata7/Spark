#include "child_visitor.hpp"

#include "node.hpp"

namespace Spark::FrontEnd {

void ChildVisitor::visit(FnParam* param) {
    add(param->mod);
    add(param->pattern);
    add(param->type);
    add(param->def);
}

void ChildVisitor::visit(FnCapture* cap) {
    add(cap->mod);
    add(cap->pattern);
}

void ChildVisitor::visit(FnCaptureClause* clause) {
    addVec(clause->captures);
    add(clause->restMod);
}

void ChildVisitor::visit(FnReturn* ret) {
    add(ret->type);
}

void ChildVisitor::visit(PathSeg* seg) {
    add(seg->name);
    addVec(seg->generics);
}

void ChildVisitor::visit(Path* path) {
    addVec(path->segs);
}

void ChildVisitor::visit(LambdaExpr* expr) {
    addVec(expr->params);
    add(expr->captureClause);
    addVec(expr->returns);
    add(expr->throwExpr);
    add(expr->body);
}

void ChildVisitor::visit(IfThenExpr* expr) {
    add(expr->condition);
    add(expr->thenExpr);
    add(expr->elseExpr);
}

void ChildVisitor::visit(TryElseExpr* expr) {
    add(expr->tryExpr);
    add(expr->elseExpr);
}

void ChildVisitor::visit(MatchCase* c) {
    add(c->pattern);
    add(c->guard);
    add(c->body);
}

void ChildVisitor::visit(MatchExpr* expr) {
    add(expr->scrutinee);
    addVec(expr->cases);
}

void ChildVisitor::visit(CatchClause* c) {
    add(c->pattern);
    add(c->guard);
    add(c->body);
}

void ChildVisitor::visit(TryCatchExpr* expr) {
    add(expr->expr);
    addVec(expr->catches);
}

void ChildVisitor::visit(ThrowExpr* expr) {
    add(expr->expr);
}

void ChildVisitor::visit(BlockExpr* expr) {
    addVec(expr->nodes);
}

void ChildVisitor::visit(IsExpr* expr) {
    add(expr->expr);
    add(expr->type);
}

void ChildVisitor::visit(AsExpr* expr) {
    add(expr->expr);
    add(expr->pattern);
    add(expr->type);
}

void ChildVisitor::visit(BinaryExpr* expr) {
    add(expr->lhs);
    add(expr->rhs);
}

void ChildVisitor::visit(PrefixExpr* expr) {
    add(expr->expr);
}

void ChildVisitor::visit(PostfixExpr* expr) {
    add(expr->expr);
}

void ChildVisitor::visit(MemberAccessExpr* expr) {
    add(expr->base);
    add(expr->member);
}

void ChildVisitor::visit(CallArg* arg) {
    add(arg->name);
    add(arg->expr);
}

void ChildVisitor::visit(CallExpr* expr) {
    add(expr->callee);
    addVec(expr->args);
}

void ChildVisitor::visit(SubscriptExpr* expr) {
    add(expr->base);
    addVec(expr->indices);
}

void ChildVisitor::visit(NameExpr* expr) {
    add(expr->name);
}

void ChildVisitor::visit(GlobalAccessExpr* expr) {
    add(expr->name);
}

void ChildVisitor::visit(UpvalueExpr* expr) {
    add(expr->name);
}

void ChildVisitor::visit(TupleExpr* expr) {
    addVec(expr->exprs);
}

void ChildVisitor::visit(CollectionExpr* expr) {
    addVec(expr->exprs);
}

void ChildVisitor::visit(TypeofExpr* expr) {
    add(expr->expr);
}

void ChildVisitor::visit(VarDefStmt* stmt) {
    add(stmt->mod);
    add(stmt->pattern);
    add(stmt->type);
    add(stmt->rhs);
}

void ChildVisitor::visit(FnDefStmt* stmt) {
    add(stmt->name);
    addVec(stmt->generics);
    addVec(stmt->params);
    add(stmt->captureClause);
    addVec(stmt->returns);
    add(stmt->throwExpr);
    add(stmt->body);
}

void ChildVisitor::visit(TypeDefStmt* stmt) {
    add(stmt->name);
    addVec(stmt->generics);
    addVec(stmt->bases);
    add(stmt->body);
}

void ChildVisitor::visit(CaseDefStmt* stmt) {
    add(stmt->name);
    add(stmt->val);
}

void ChildVisitor::visit(AssignStmt* stmt) {
    add(stmt->lhs);
    add(stmt->rhs);
}

void ChildVisitor::visit(IfStmt* stmt) {
    add(stmt->condition);
    add(stmt->thenBody);
    add(stmt->elseBody);
}

void ChildVisitor::visit(WhileStmt* stmt) {
    add(stmt->condition);
    add(stmt->body);
}

void ChildVisitor::visit(DoWhileStmt* stmt) {
    add(stmt->body);
    add(stmt->condition);
}

void ChildVisitor::visit(ForStmt* stmt) {
    add(stmt->iterator);
    add(stmt->range);
    add(stmt->body);
}

void ChildVisitor::visit(ReturnStmt* stmt) {
    add(stmt->expr);
}

void ChildVisitor::visit(ModuleStmt* stmt) {
    add(stmt->path);
    add(stmt->body);
}

void ChildVisitor::visit(ExportStmt* stmt) {
    add(stmt->stmt);
}

void ChildVisitor::visit(ImportItem* stmt) {
    add(stmt->path);
    add(stmt->as);
}

void ChildVisitor::visit(ImportStmt* stmt) {
    add(stmt->from);
    addVec(stmt->imports);
}

void ChildVisitor::visit(ImportAllStmt* stmt) {
    add(stmt->from);
}

void ChildVisitor::visit(UndefineStmt* stmt) {
    add(stmt->path);
}

void ChildVisitor::visit(BindingPattern* pattern) {
    add(pattern->name);
}

void ChildVisitor::visit(TuplePattern* pattern) {
    addVec(pattern->patterns);
}

void ChildVisitor::visit(CollectionPattern* pattern) {
    addVec(pattern->prefix);
    addVec(pattern->suffix);
}

void ChildVisitor::visit(RecordPatternField* pattern) {
    add(pattern->label);
    add(pattern->pattern);
}

void ChildVisitor::visit(RecordPattern* pattern) {
    addVec(pattern->fields);
}

} // Spark::FrontEnd
