#pragma once

namespace Spark::FrontEnd::AST {

struct Node;

struct IdentifierNode;

struct UnaryExpr;
struct BinaryExpr;
struct IfThenExpr;

struct BlockStmt;
struct LetStmt;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(Node& node) { }
    virtual void visit(IdentifierNode& node) { }
    virtual void visit(UnaryExpr& node) { }
    virtual void visit(BinaryExpr& node) { }
    virtual void visit(IfThenExpr& node) { }
    virtual void visit(BlockStmt& node) { }
    virtual void visit(LetStmt& node) { }
};

} // Spark::FrontEnd::AST
