#include "node_visitor.hpp"

#include "node.hpp"

namespace Spark::FrontEnd {

void NodeVisitor::visit(Node& node) { }

void NodeVisitor::visit(Expr& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(Stmt& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(Pattern& node) { visit(static_cast<Node&>(node)); }

void NodeVisitor::visit(VarModifier& node) { visit(static_cast<Node&>(node)); }

void NodeVisitor::visit(FnParam& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(FnCapture& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(FnCaptureClause& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(FnReturn& node) { visit(static_cast<Node&>(node)); }

void NodeVisitor::visit(PathSeg& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(Path& node) { visit(static_cast<Node&>(node)); }

void NodeVisitor::visit(LambdaExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(IfThenExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(TryElseExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(MatchCase& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(MatchExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(CatchClause& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(TryCatchExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(ThrowExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(BlockExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(IsExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(AsExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(BinaryExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(PrefixExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(PostfixExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(MemberAccessExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(CallArg& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(CallExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(SubscriptExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(LiteralExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(NameExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(GlobalAccessExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(UpvalueExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(TupleExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(CollectionExpr& node) { visit(static_cast<Expr&>(node)); }
void NodeVisitor::visit(TypeofExpr& node) { visit(static_cast<Expr&>(node)); }

void NodeVisitor::visit(VarDefStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(FnDefStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(TypeDefStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(CaseDefStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(AssignStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(IfStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(WhileStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(DoWhileStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(ForStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(BreakStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(ContinueStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(ReturnStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(ModuleStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(ExportStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(ImportItem& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(ImportStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(ImportAllStmt& node) { visit(static_cast<Stmt&>(node)); }
void NodeVisitor::visit(UndefineStmt& node) { visit(static_cast<Stmt&>(node)); }

void NodeVisitor::visit(LiteralPattern& node) { visit(static_cast<Pattern&>(node)); }
void NodeVisitor::visit(BindingPattern& node) { visit(static_cast<Pattern&>(node)); }
void NodeVisitor::visit(TuplePattern& node) { visit(static_cast<Pattern&>(node)); }
void NodeVisitor::visit(CollectionPattern& node) { visit(static_cast<Pattern&>(node)); }
void NodeVisitor::visit(RecordPatternField& node) { visit(static_cast<Node&>(node)); }
void NodeVisitor::visit(RecordPattern& node) { visit(static_cast<Pattern&>(node)); }

} // Spark::FrontEnd
