#include "node_visitor.hpp"

#include "node.hpp"

namespace Spark::FrontEnd {

void NodeVisitor::visit(Node* node) { }

void NodeVisitor::visit(Expr* expr) { visit(static_cast<Node*>(expr)); }
void NodeVisitor::visit(Stmt* stmt) { visit(static_cast<Node*>(stmt)); }
void NodeVisitor::visit(Pattern* pattern) { visit(static_cast<Node*>(pattern)); }

void NodeVisitor::visit(Name* name) { visit(static_cast<Node*>(name)); }

void NodeVisitor::visit(VarModifier* varmod) { visit(static_cast<Node*>(varmod)); }

void NodeVisitor::visit(FnParam* param) { visit(static_cast<Node*>(param)); }
void NodeVisitor::visit(FnCapture* cap) { visit(static_cast<Node*>(cap)); }
void NodeVisitor::visit(FnCaptureClause* clause) { visit(static_cast<Node*>(clause)); }
void NodeVisitor::visit(FnReturn* ret) { visit(static_cast<Node*>(ret)); }

void NodeVisitor::visit(PathSeg* seg) { visit(static_cast<Node*>(seg)); }
void NodeVisitor::visit(Path* path) { visit(static_cast<Node*>(path)); }

void NodeVisitor::visit(LambdaExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(IfThenExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(TryElseExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(MatchCase* c) { visit(static_cast<Node*>(c)); }
void NodeVisitor::visit(MatchExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(CatchClause* c) { visit(static_cast<Node*>(c)); }
void NodeVisitor::visit(TryCatchExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(ThrowExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(BlockExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(IsExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(AsExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(BinaryExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(PrefixExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(PostfixExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(MemberAccessExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(CallArg* arg) { visit(static_cast<Node*>(arg)); }
void NodeVisitor::visit(CallExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(SubscriptExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(LiteralExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(NameExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(GlobalAccessExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(UpvalueExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(TupleExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(CollectionExpr* expr) { visit(static_cast<Expr*>(expr)); }
void NodeVisitor::visit(TypeofExpr* expr) { visit(static_cast<Expr*>(expr)); }

void NodeVisitor::visit(VarDefStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(FnDefStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(TypeDefStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(CaseDefStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(AssignStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(IfStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(WhileStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(DoWhileStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(ForStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(BreakStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(ContinueStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(ReturnStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(ModuleStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(ExportStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(ImportItem* stmt) { visit(static_cast<Node*>(stmt)); }
void NodeVisitor::visit(ImportStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(ImportAllStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }
void NodeVisitor::visit(UndefineStmt* stmt) { visit(static_cast<Stmt*>(stmt)); }

void NodeVisitor::visit(LiteralPattern* pattern) { visit(static_cast<Pattern*>(pattern)); }
void NodeVisitor::visit(BindingPattern* pattern) { visit(static_cast<Pattern*>(pattern)); }
void NodeVisitor::visit(TuplePattern* pattern) { visit(static_cast<Pattern*>(pattern)); }
void NodeVisitor::visit(CollectionPattern* pattern) { visit(static_cast<Pattern*>(pattern)); }
void NodeVisitor::visit(RecordPatternField* field) { visit(static_cast<Node*>(field)); }
void NodeVisitor::visit(RecordPattern* pattern) { visit(static_cast<Pattern*>(pattern)); }

} // Spark::FrontEnd
