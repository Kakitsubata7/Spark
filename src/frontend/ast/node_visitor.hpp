#pragma once

namespace Spark::FrontEnd::AST {

struct Node;

struct Pattern;
struct VarPattern;
struct TuplePattern;

struct Expr;
struct VarExpr;
struct SequenceExpr;
struct AssignExpr;
struct UnaryExpr;
struct BinaryExpr;
struct IfThenExpr;
struct ThrowExpr;
struct TryElseExpr;

struct Stmt;
struct BlockStmt;
struct VarDeclStmt;
struct RefDeclStmt;
struct AssignStmt;
struct IfStmt;
struct MatchStmt;
struct WhileStmt;
struct ForStmt;
struct BreakStmt;
struct ContinueStmt;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(Node& node) { }

    virtual void visit(Pattern& node) { }
    virtual void visit(VarPattern& node) { }
    virtual void visit(TuplePattern& node) { }

    virtual void visit(Expr& node) { }
    virtual void visit(VarExpr& node) { }
    virtual void visit(SequenceExpr& node) { }
    virtual void visit(AssignExpr& node) { }
    virtual void visit(UnaryExpr& node) { }
    virtual void visit(BinaryExpr& node) { }
    virtual void visit(IfThenExpr& node) { }
    virtual void visit(ThrowExpr& node) { }
    virtual void visit(TryElseExpr& node) { }

    virtual void visit(Stmt& node) { }
    virtual void visit(BlockStmt& node) { }
    virtual void visit(VarDeclStmt& node) { }
    virtual void visit(RefDeclStmt& node) { }
    virtual void visit(AssignStmt& node) { }
    virtual void visit(IfStmt& node) { }
    virtual void visit(MatchStmt& node) { }
    virtual void visit(WhileStmt& node) { }
    virtual void visit(ForStmt& node) { }
    virtual void visit(BreakStmt& node) { }
    virtual void visit(ContinueStmt& node) { }
};

} // Spark::FrontEnd::AST
