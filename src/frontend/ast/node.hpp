#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "node_visitor.hpp"
#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"
#include "utils/location.hpp"

namespace Spark::FrontEnd {

enum class NodeKind : uint32_t {
    Other   = 0,
    Expr    = 1 << 0,
    Stmt    = 1 << 1,
    Pattern = 1 << 2
};

inline NodeKind operator|(NodeKind lhs, NodeKind rhs) {
    return static_cast<NodeKind>(
        static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
    );
}

inline NodeKind operator&(NodeKind lhs, NodeKind rhs) {
    return static_cast<NodeKind>(
        static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
    );
}

inline NodeKind& operator|=(NodeKind& lhs, NodeKind rhs) {
    lhs = lhs | rhs;
    return lhs;
}

class NodeVisitor;

struct Node {
    Location start;
    Location end;

    Node(Location start, Location end) noexcept
        : start(start), end(end) { }

    virtual ~Node() = default;

    [[nodiscard]]
    virtual NodeKind kind() const = 0;

    virtual void accept(NodeVisitor& v) = 0;

    bool operator==(const Node& rhs) const noexcept;
    bool operator!=(const Node& rhs) const noexcept;

protected:
    [[nodiscard]]
    virtual bool equalsImpl(const Node& rhs) const noexcept = 0;
};

struct Body final : Node {
    std::vector<Node*> nodes;

    Body(Location start, Location end, std::vector<Node*> nodes = {}) noexcept
        : Node(start, end), nodes(std::move(nodes)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BlockStmt final : Node {
    Body* body;

    BlockStmt(Location start, Location end, Body* body) noexcept
        : Node(start, end), body(body) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct IfElseStmt final : Node {
    struct Branch {
        Node* condition;
        BlockStmt* block;
    };

    std::vector<Branch> ifBranches;
    /* nullable */ BlockStmt* elseBlock;

    IfElseStmt(Location start, Location end, std::vector<Branch> ifBranches, BlockStmt* elseBlock = nullptr) noexcept
        : Node(start, end), ifBranches(std::move(ifBranches)), elseBlock(elseBlock) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct WhileStmt final : Node {
    Node* condition;
    BlockStmt* block;

    WhileStmt(Location start, Location end, Node* condition, BlockStmt* block) noexcept
        : Node(start, end), condition(condition), block(block) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ForStmt final : Node {
    Node* iterator;
    Node* range;
    BlockStmt* block;

    ForStmt(Location start, Location end, Node* iterator, Node* range, BlockStmt* block) noexcept
        : Node(start, end), iterator(iterator), range(range), block(block) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BreakStmt final : Node {
    BreakStmt(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ContinueStmt final : Node {
    ContinueStmt(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct IfThenExpr final : Node {
    Node* condition;
    Node* trueNode;
    Node* falseNode;

    IfThenExpr(Location start, Location end, Node* condition, Node* trueNode, Node* falseNode) noexcept
        : Node(start, end), condition(condition), trueNode(trueNode), falseNode(falseNode) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

enum class PrefixOp {
    None, Pos, Neg, BitNot, LogNot, Ref
};

struct PrefixExpr final : Node {
    PrefixOp op;
    Node* expr;

    PrefixExpr(Location start, Location end, PrefixOp op, Node* expr) noexcept
         : Node(start, end), op(op), expr(expr) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

enum class PostfixOp {
    None, Optional, ForceUnwrap
};

struct PostfixExpr final : Node {
    PostfixOp op;
    Node* expr;

    PostfixExpr(Location start, Location end, PostfixOp op, Node* expr) noexcept
         : Node(start, end), op(op), expr(expr) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

enum class VarKind {
    None, Let, Const, Ref, Cref
};

struct BindingModifier {
    VarKind kind;
    bool isImmutable;
    bool isNullable;
    bool isNonNullable;

    explicit BindingModifier(VarKind kind = VarKind::None,
                         bool isImmutable = false,
                         bool isNullable = false,
                         bool isNonNullable = false) noexcept
        : kind(kind), isImmutable(isImmutable), isNullable(isNullable), isNonNullable(isNonNullable) { }

    bool operator==(const BindingModifier& rhs) const noexcept;
    bool operator!=(const BindingModifier& rhs) const noexcept;
};

struct BindingPattern final : Node {
    BindingModifier mod;
    Node* bind;
    /* nullable */ Node* typeAnnot;

    BindingPattern(Location start, Location end, BindingModifier mod, Node* bind, Node* typeAnnot = nullptr) noexcept
        : Node(start, end), mod(mod), bind(bind), typeAnnot(typeAnnot) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct TuplePattern final : Node {
    /* notempty */ std::vector<Node*> patterns;

    TuplePattern(Location start, Location end, std::vector<Node*> patterns) noexcept
        : Node(start, end), patterns(std::move(patterns)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct CollectionPattern final : Node {
    std::vector<Node*> prefix;
    std::vector<Node*> suffix;
    bool hasRest;

    CollectionPattern(Location start,
                      Location end,
                      std::vector<Node*> prefix = {},
                      std::vector<Node*> suffix = {},
                      bool hasRest = false) noexcept
        : Node(start, end), prefix(std::move(prefix)), suffix(std::move(suffix)), hasRest(hasRest) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Identifier final : Node {
    std::string name;

    Identifier(Location start, Location end, std::string name) noexcept
        : Node(start, end), name(std::move(name)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Discard final : Node {
    Discard(Location start, Location end) noexcept
        : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct IntLiteral final : Node {
    BigInt value;

    IntLiteral(Location start, Location end, BigInt value) noexcept
        : Node(start, end), value(std::move(value)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct RealLiteral final : Node {
    BigReal value;

    RealLiteral(Location start, Location end, BigReal value) noexcept
        : Node(start, end), value(std::move(value)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BoolLiteral final : Node {
    bool value;

    BoolLiteral(Location start, Location end, bool value) noexcept
        : Node(start, end), value(value) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct StringLiteral final : Node {
    std::string value;

    StringLiteral(Location start, Location end, std::string value) noexcept
        : Node(start, end), value(std::move(value)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct NilLiteral final : Node {
    NilLiteral(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct VoidLiteral final : Node {
    VoidLiteral(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Undefined final : Node {
    Undefined(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

} // Spark::FrontEnd
