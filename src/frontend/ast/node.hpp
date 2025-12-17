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

struct BodyNode final : Node {
    std::vector<Node*> nodes;

    BodyNode(Location start, Location end, std::vector<Node*> nodes = {}) noexcept
        : Node(start, end), nodes(std::move(nodes)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd
