#pragma once

namespace Spark::FrontEnd {

struct Node;

struct Expr;
struct Stmt;
struct Pattern;

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

    virtual void visit(Node& node) { }

    virtual void visit(Expr& node) { }
    virtual void visit(Stmt& node) { }
    virtual void visit(Pattern& node) { }

    virtual void visit(VarModifier& node) { }

    virtual void visit(FnParam& node) { }
    virtual void visit(FnCapture& node) { }
    virtual void visit(FnCaptureClause& node) { }
    virtual void visit(FnReturn& node) { }

    virtual void visit(PathSeg& node) { }
    virtual void visit(Path& node) { }

    virtual void visit(LambdaExpr& node) { }
    virtual void visit(IfThenExpr& node) { }
    virtual void visit(TryElseExpr& node) { }
    virtual void visit(MatchCase& node) { }
    virtual void visit(MatchExpr& node) { }
    virtual void visit(CatchClause& node) { }
    virtual void visit(TryCatchExpr& node) { }
    virtual void visit(ThrowExpr& node) { }
    virtual void visit(BlockExpr& node) { }
    virtual void visit(IsExpr& node) { }
    virtual void visit(AsExpr& node) { }
    virtual void visit(BinaryExpr& node) { }
    virtual void visit(PrefixExpr& node) { }
    virtual void visit(PostfixExpr& node) { }
    virtual void visit(MemberAccessExpr& node) { }
    virtual void visit(CallArg& node) { }
    virtual void visit(CallExpr& node) { }
    virtual void visit(SubscriptExpr& node) { }
    virtual void visit(LiteralExpr& node) { }
    virtual void visit(NameExpr& node) { }
    virtual void visit(GlobalAccessExpr& node) { }
    virtual void visit(UpvalueExpr& node) { }
    virtual void visit(TupleExpr& node) { }
    virtual void visit(CollectionExpr& node) { }
    virtual void visit(TypeofExpr& node) { }

    virtual void visit(VarDefStmt& node) { }
    virtual void visit(FnDefStmt& node) { }
    virtual void visit(TypeDefStmt& node) { }
    virtual void visit(CaseDefStmt& node) { }
    virtual void visit(AssignStmt& node) { }
    virtual void visit(IfStmt& node) { }
    virtual void visit(WhileStmt& node) { }
    virtual void visit(DoWhileStmt& node) { }
    virtual void visit(ForStmt& node) { }
    virtual void visit(BreakStmt& node) { }
    virtual void visit(ContinueStmt& node) { }
    virtual void visit(ReturnStmt& node) { }
    virtual void visit(ModuleStmt& node) { }
    virtual void visit(ExportStmt& node) { }
    virtual void visit(ImportItem& node) { }
    virtual void visit(ImportStmt& node) { }
    virtual void visit(ImportAllStmt& node) { }
    virtual void visit(UndefineStmt& node) { }

    virtual void visit(LiteralPattern& node) { }
    virtual void visit(BindingPattern& node) { }
    virtual void visit(TuplePattern& node) { }
    virtual void visit(CollectionPattern& node) { }
    virtual void visit(RecordPatternField& node) { }
    virtual void visit(RecordPattern& node) { }
};

} // Spark::FrontEnd
