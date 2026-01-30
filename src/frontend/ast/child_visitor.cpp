#include "child_visitor.hpp"

#include "node.hpp"

namespace Spark::FrontEnd {

void ChildVisitor::visit(FnParam& node) {
    add(node.mod);
    add(node.pattern);
    add(node.type);
    add(node.def);
}

void ChildVisitor::visit(FnCapture& node) {
    add(node.mod);
    add(node.pattern);
}

void ChildVisitor::visit(FnCaptureClause& node) {
    addVec(node.captures);
    add(node.restMod);
}

void ChildVisitor::visit(FnReturn& node) {
    add(node.type);
}

void ChildVisitor::visit(PathSeg& node) {
    addVec(node.generics);
}

void ChildVisitor::visit(Path& node) {
    addVec(node.segs);
}

void ChildVisitor::visit(LambdaExpr& node) {
    addVec(node.params);
    add(node.captureClause);
    addVec(node.returns);
    add(node.throwExpr);
    add(node.body);
}

void ChildVisitor::visit(IfThenExpr& node) {
    add(node.condition);
    add(node.thenExpr);
    add(node.elseExpr);
}

void ChildVisitor::visit(TryElseExpr& node) {
    add(node.tryExpr);
    add(node.elseExpr);
}

void ChildVisitor::visit(MatchCase& node) {
    add(node.pattern);
    add(node.guard);
    add(node.body);
}

void ChildVisitor::visit(MatchExpr& node) {
    add(node.scrutinee);
    addVec(node.cases);
}

void ChildVisitor::visit(CatchClause& node) {
    add(node.pattern);
    add(node.guard);
    add(node.body);
}

void ChildVisitor::visit(TryCatchExpr& node) {
    add(node.expr);
    addVec(node.catches);
}

void ChildVisitor::visit(ThrowExpr& node) {
    add(node.expr);
}

void ChildVisitor::visit(BlockExpr& node) {
    addVec(node.nodes);
}

void ChildVisitor::visit(IsExpr& node) {
    add(node.expr);
    add(node.type);
}

void ChildVisitor::visit(AsExpr& node) {
    add(node.expr);
    add(node.pattern);
    add(node.type);
}

void ChildVisitor::visit(BinaryExpr& node) {
    add(node.lhs);
    add(node.rhs);
}

void ChildVisitor::visit(PrefixExpr& node) {
    add(node.expr);
}

void ChildVisitor::visit(PostfixExpr& node) {
    add(node.expr);
}

void ChildVisitor::visit(MemberAccessExpr& node) {
    add(node.base);
}

void ChildVisitor::visit(CallArg& node) {
    add(node.expr);
}

void ChildVisitor::visit(CallExpr& node) {
    add(node.callee);
    addVec(node.args);
}

void ChildVisitor::visit(SubscriptExpr& node) {
    add(node.base);
    addVec(node.indices);
}

void ChildVisitor::visit(TupleExpr& node) {
    addVec(node.exprs);
}

void ChildVisitor::visit(CollectionExpr& node) {
    addVec(node.exprs);
}

void ChildVisitor::visit(TypeofExpr& node) {
    add(node.expr);
}

void ChildVisitor::visit(VarDefStmt& node) {
    add(node.mod);
    add(node.pattern);
    add(node.type);
    add(node.rhs);
}

void ChildVisitor::visit(FnDefStmt& node) {
    addVec(node.params);
    add(node.captureClause);
    addVec(node.returns);
    add(node.throwExpr);
    add(node.body);
}

void ChildVisitor::visit(TypeDefStmt& node) {
    addVec(node.bases);
    add(node.body);
}

void ChildVisitor::visit(CaseDefStmt& node) {
    add(node.val);
}

void ChildVisitor::visit(AssignStmt& node) {
    add(node.lhs);
    add(node.rhs);
}

void ChildVisitor::visit(IfStmt& node) {
    add(node.condition);
    add(node.thenBody);
    add(node.elseBody);
}

void ChildVisitor::visit(WhileStmt& node) {
    add(node.condition);
    add(node.body);
}

void ChildVisitor::visit(DoWhileStmt& node) {
    add(node.body);
    add(node.condition);
}

void ChildVisitor::visit(ForStmt& node) {
    add(node.iterator);
    add(node.range);
    add(node.body);
}

void ChildVisitor::visit(ReturnStmt& node) {
    add(node.expr);
}

void ChildVisitor::visit(ModuleStmt& node) {
    add(node.path);
    add(node.body);
}

void ChildVisitor::visit(ExportStmt& node) {
    add(node.stmt);
}

void ChildVisitor::visit(ImportItem& node) {
    add(node.path);
}

void ChildVisitor::visit(ImportStmt& node) {
    add(node.from);
    addVec(node.imports);
}

void ChildVisitor::visit(ImportAllStmt& node) {
    add(node.from);
}

void ChildVisitor::visit(UndefineStmt& node) {
    add(node.path);
}

void ChildVisitor::visit(TuplePattern& node) {
    addVec(node.patterns);
}

void ChildVisitor::visit(CollectionPattern& node) {
    addVec(node.prefix);
    addVec(node.suffix);
}

void ChildVisitor::visit(RecordPatternField& node) {
    add(node.pattern);
}

void ChildVisitor::visit(RecordPattern& node) {
    addVec(node.fields);
}

} // Spark::FrontEnd
