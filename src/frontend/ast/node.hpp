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
};

struct Body final : Node {
    std::vector<Node*> nodes;

    Body(Location start, Location end, std::vector<Node*> nodes = {}) noexcept
        : Node(start, end), nodes(std::move(nodes)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct BlockStmt final : Node {
    Body* body;

    BlockStmt(Location start, Location end, Body* body) noexcept
        : Node(start, end), body(body) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct WhileStmt final : Node {
    Node* condition;
    BlockStmt* block;

    WhileStmt(Location start, Location end, Node* condition, BlockStmt* block) noexcept
        : Node(start, end), condition(condition), block(block) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
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
};

struct TuplePattern final : Node {
    /* notempty */ std::vector<Node*> patterns;

    TuplePattern(Location start, Location end, std::vector<Node*> patterns) noexcept
        : Node(start, end), patterns(std::move(patterns)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
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
};

struct Identifier final : Node {
    std::string name;

    Identifier(Location start, Location end, std::string name) noexcept
        : Node(start, end), name(std::move(name)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Discard final : Node {
    Discard(Location start, Location end) noexcept
        : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct IntLiteral final : Node {
    BigInt value;

    IntLiteral(Location start, Location end, BigInt value) noexcept
        : Node(start, end), value(std::move(value)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct RealLiteral final : Node {
    BigReal value;

    RealLiteral(Location start, Location end, BigReal value) noexcept
        : Node(start, end), value(std::move(value)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct BoolLiteral final : Node {
    bool value;

    BoolLiteral(Location start, Location end, bool value) noexcept
        : Node(start, end), value(value) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct StringLiteral final : Node {
    std::string value;

    StringLiteral(Location start, Location end, std::string value) noexcept
        : Node(start, end), value(std::move(value)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct NilLiteral final : Node {
    NilLiteral(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Undefined final : Node {
    Undefined(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd
