#pragma once

namespace Spark::FrontEnd {

struct Node;

struct BodyNode;

struct BlockStmt;
struct WhileStmt;

struct IfThenExpr;

struct IntLiteral;
struct RealLiteral;
struct BoolLiteral;
struct StringLiteral;
struct NilLiteral;
struct UndefinedExpr;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(Node& node) { }

    virtual void visit(BodyNode& node) { }

    virtual void visit(BlockStmt& node) { }
    virtual void visit(WhileStmt& node) { }

    virtual void visit(IfThenExpr& node) { }

    virtual void visit(IntLiteral& node) { }
    virtual void visit(RealLiteral& node) { }
    virtual void visit(BoolLiteral& node) { }
    virtual void visit(StringLiteral& node) { }
    virtual void visit(NilLiteral& node) { }
    virtual void visit(UndefinedExpr& node) { }
};

} // Spark::FrontEnd
