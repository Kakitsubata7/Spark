#pragma once

namespace Spark::FrontEnd {

struct Node;

struct BodyNode;

struct BlockStmt;
struct WhileStmt;

struct IfThenExpr;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(Node& node) { }

    virtual void visit(BodyNode& node) { }

    virtual void visit(BlockStmt& node) { }
    virtual void visit(WhileStmt& node) { }

    virtual void visit(IfThenExpr& node) { }
};

} // Spark::FrontEnd
