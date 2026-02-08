#pragma once

namespace Spark::FrontEnd {

struct Node;

struct Expr;
struct Stmt;
struct Pattern;

struct Name;

struct VarModifier;

struct FnParam;
struct FnCapture;
struct FnCaptureClause;
struct FnReturn;

struct PathSeg;
struct Path;

struct LambdaExpr;
struct IfThenExpr;
struct TryElseExpr;
struct MatchCase;
struct MatchExpr;
struct CatchClause;
struct TryCatchExpr;
struct ThrowExpr;
struct BlockExpr;
struct IsExpr;
struct AsExpr;
struct BinaryExpr;
struct PrefixExpr;
struct PostfixExpr;
struct MemberAccessExpr;
struct CallArg;
struct CallExpr;
struct SubscriptExpr;
struct LiteralExpr;
struct NameExpr;
struct GlobalAccessExpr;
struct UpvalueExpr;
struct TupleExpr;
struct CollectionExpr;
struct TypeofExpr;

struct VarDefStmt;
struct FnDefStmt;
struct TypeDefStmt;
struct CaseDefStmt;
struct AssignStmt;
struct IfStmt;
struct WhileStmt;
struct DoWhileStmt;
struct ForStmt;
struct BreakStmt;
struct ContinueStmt;
struct ReturnStmt;
struct ModuleStmt;
struct ExportStmt;
struct ImportItem;
struct ImportStmt;
struct ImportAllStmt;
struct UndefineStmt;

struct LiteralPattern;
struct BindingPattern;
struct TuplePattern;
struct CollectionPattern;
struct RecordPatternField;
struct RecordPattern;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(Node* node);

    virtual void visit(Expr* expr); 
    virtual void visit(Stmt* stmt);
    virtual void visit(Pattern* pattern);

    virtual void visit(Name* name);

    virtual void visit(VarModifier* varmod);

    virtual void visit(FnParam* param);
    virtual void visit(FnCapture* cap);
    virtual void visit(FnCaptureClause* clause);
    virtual void visit(FnReturn* ret);

    virtual void visit(PathSeg* seg);
    virtual void visit(Path* path);

    virtual void visit(LambdaExpr* expr);
    virtual void visit(IfThenExpr* expr);
    virtual void visit(TryElseExpr* expr);
    virtual void visit(MatchCase* c);
    virtual void visit(MatchExpr* expr);
    virtual void visit(CatchClause* c);
    virtual void visit(TryCatchExpr* expr);
    virtual void visit(ThrowExpr* expr);
    virtual void visit(BlockExpr* expr);
    virtual void visit(IsExpr* expr);
    virtual void visit(AsExpr* expr);
    virtual void visit(BinaryExpr* expr);
    virtual void visit(PrefixExpr* expr);
    virtual void visit(PostfixExpr* expr);
    virtual void visit(MemberAccessExpr* expr);
    virtual void visit(CallArg* arg);
    virtual void visit(CallExpr* expr);
    virtual void visit(SubscriptExpr* expr);
    virtual void visit(LiteralExpr* expr);
    virtual void visit(NameExpr* expr);
    virtual void visit(GlobalAccessExpr* expr);
    virtual void visit(UpvalueExpr* expr);
    virtual void visit(TupleExpr* expr);
    virtual void visit(CollectionExpr* expr);
    virtual void visit(TypeofExpr* expr);

    virtual void visit(VarDefStmt* stmt);
    virtual void visit(FnDefStmt* stmt);
    virtual void visit(TypeDefStmt* stmt);
    virtual void visit(CaseDefStmt* stmt);
    virtual void visit(AssignStmt* stmt);
    virtual void visit(IfStmt* stmt);
    virtual void visit(WhileStmt* stmt);
    virtual void visit(DoWhileStmt* stmt);
    virtual void visit(ForStmt* stmt);
    virtual void visit(BreakStmt* stmt);
    virtual void visit(ContinueStmt* stmt);
    virtual void visit(ReturnStmt* stmt);
    virtual void visit(ModuleStmt* stmt);
    virtual void visit(ExportStmt* stmt);
    virtual void visit(ImportItem* stmt);
    virtual void visit(ImportStmt* stmt);
    virtual void visit(ImportAllStmt* stmt);
    virtual void visit(UndefineStmt* stmt);

    virtual void visit(LiteralPattern* pattern);
    virtual void visit(BindingPattern* pattern);
    virtual void visit(TuplePattern* pattern);
    virtual void visit(CollectionPattern* pattern);
    virtual void visit(RecordPatternField* field);
    virtual void visit(RecordPattern* pattern);
};

} // Spark::FrontEnd
