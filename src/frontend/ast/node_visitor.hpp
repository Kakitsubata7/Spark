#pragma once

namespace Spark::FrontEnd::AST {

struct Node;

struct UnaryExpr;
struct BinaryExpr;
struct IfThenExpr;

struct BlockStmt;
struct LetStmt;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(Node& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
    virtual void visit(BinaryExpr& node) = 0;
    virtual void visit(IfThenExpr& node) = 0;
    virtual void visit(BlockStmt& node) = 0;
    virtual void visit(LetStmt& node) = 0;
};

} // Spark::FrontEnd::AST
