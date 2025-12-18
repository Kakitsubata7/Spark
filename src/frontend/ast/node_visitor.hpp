#pragma once

namespace Spark::FrontEnd {

struct Node;

struct Body;

struct BlockStmt;
struct WhileStmt;
struct ForStmt;
struct BreakStmt;
struct ContinueStmt;

struct IfThenExpr;

struct BindingPattern;
struct TuplePattern;
struct CollectionPattern;

struct Identifier;
struct Discard;

struct IntLiteral;
struct RealLiteral;
struct BoolLiteral;
struct StringLiteral;
struct NilLiteral;
struct Undefined;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(Node& node) { }

    virtual void visit(Body& node) { }

    virtual void visit(BlockStmt& node) { }
    virtual void visit(WhileStmt& node) { }
    virtual void visit(ForStmt& node) { }
    virtual void visit(BreakStmt& node) { }
    virtual void visit(ContinueStmt& node) { }

    virtual void visit(IfThenExpr& node) { }

    virtual void visit(BindingPattern& node) { }
    virtual void visit(TuplePattern& node) { }
    virtual void visit(CollectionPattern& node) { }

    virtual void visit(Identifier& node) { }
    virtual void visit(Discard& node) { }

    virtual void visit(IntLiteral& node) { }
    virtual void visit(RealLiteral& node) { }
    virtual void visit(BoolLiteral& node) { }
    virtual void visit(StringLiteral& node) { }
    virtual void visit(NilLiteral& node) { }
    virtual void visit(Undefined& node) { }
};

} // Spark::FrontEnd
