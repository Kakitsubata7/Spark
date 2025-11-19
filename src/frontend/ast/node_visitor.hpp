#pragma once

namespace Spark::FrontEnd::AST {

struct Node;

struct TypeNode; struct TypeSegment; struct TypePath;

struct Pattern;
struct IntLiteralPattern;
struct RealLiteralPattern;
struct BoolLiteralPattern;
struct StringLiteralPattern;
struct NilLiteralPattern;
struct WildcardPattern;
struct VarPattern;
struct TuplePattern;
struct CollectionPattern;
struct FieldPattern;
struct OrPattern;

struct Expr;
struct IntLiteralExpr;
struct RealLiteralExpr;
struct BoolLiteralExpr;
struct StringLiteralExpr;
struct NilLiteralExpr;
struct VarExpr;
struct FnExpr;
struct LambdaExpr;
struct AssignExpr;
struct UnaryExpr;
struct ForceUnwrapExpr;
struct BinaryExpr;
struct FieldAccessExpr;
struct CallExpr;
struct SubscriptExpr;
struct RangeExpr;
struct IfThenExpr;
struct MatchExpr;
struct ThrowExpr;
struct TryElseExpr;

struct Stmt;
struct BlockStmt;
struct VarDeclStmt;
struct FnDeclStmt;
struct LambdaDeclStmt;
struct RefDeclStmt;
struct AssignStmt;
struct CallStmt;
struct SubscriptStmt;
struct IfStmt;
struct MatchStmt;
struct WhileStmt;
struct ForStmt;
struct BreakStmt;
struct ContinueStmt;
struct ThrowStmt;
struct TryStmt;
struct TryCatchStmt;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(Node& node) { }

    virtual void visit(TypeNode& node) { }
    virtual void visit(TypeSegment& node) { }
    virtual void visit(TypePath& node) { }

    virtual void visit(Pattern& node) { }
    virtual void visit(IntLiteralPattern& node) { }
    virtual void visit(RealLiteralPattern& node) { }
    virtual void visit(BoolLiteralPattern& node) { }
    virtual void visit(StringLiteralPattern& node) { }
    virtual void visit(NilLiteralPattern& node) { }
    virtual void visit(WildcardPattern& node) { }
    virtual void visit(VarPattern& node) { }
    virtual void visit(TuplePattern& node) { }
    virtual void visit(CollectionPattern& node) { }
    virtual void visit(FieldPattern& node) { }
    virtual void visit(OrPattern& node) { }

    virtual void visit(Expr& node) { }
    virtual void visit(IntLiteralExpr& node) { }
    virtual void visit(RealLiteralExpr& node) { }
    virtual void visit(BoolLiteralExpr& node) { }
    virtual void visit(StringLiteralExpr& node) { }
    virtual void visit(NilLiteralExpr& node) { }
    virtual void visit(VarExpr& node) { }
    virtual void visit(FnExpr& node) { }
    virtual void visit(LambdaExpr& node) { }
    virtual void visit(AssignExpr& node) { }
    virtual void visit(UnaryExpr& node) { }
    virtual void visit(ForceUnwrapExpr& node) { }
    virtual void visit(BinaryExpr& node) { }
    virtual void visit(FieldAccessExpr& node) { }
    virtual void visit(CallExpr& node) { }
    virtual void visit(SubscriptExpr& node) { }
    virtual void visit(RangeExpr& node) { }
    virtual void visit(IfThenExpr& node) { }
    virtual void visit(MatchExpr& node) { }
    virtual void visit(ThrowExpr& node) { }
    virtual void visit(TryElseExpr& node) { }

    virtual void visit(Stmt& node) { }
    virtual void visit(BlockStmt& node) { }
    virtual void visit(VarDeclStmt& node) { }
    virtual void visit(FnDeclStmt& node) { }
    virtual void visit(LambdaDeclStmt& node) { }
    virtual void visit(RefDeclStmt& node) { }
    virtual void visit(AssignStmt& node) { }
    virtual void visit(CallStmt& node) { }
    virtual void visit(SubscriptStmt& node) { }
    virtual void visit(IfStmt& node) { }
    virtual void visit(MatchStmt& node) { }
    virtual void visit(WhileStmt& node) { }
    virtual void visit(ForStmt& node) { }
    virtual void visit(BreakStmt& node) { }
    virtual void visit(ContinueStmt& node) { }
    virtual void visit(TryStmt& node) { }
    virtual void visit(ThrowStmt& node) { }
    virtual void visit(TryCatchStmt& node) { }
};

} // Spark::FrontEnd::AST
