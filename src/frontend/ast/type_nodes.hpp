#pragma once

#include <string>
#include <vector>

#include "node.hpp"

namespace Spark::FrontEnd {

struct TypeNode : Node {
    TypeNode(Location start, Location end) noexcept : Node(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct TypeSegment final : TypeNode {
    std::string name;
    bool isNullable;
    bool isImmutable;

    TypeSegment(Location start, Location end, std::string name, bool isImmutable, bool isNullable) noexcept
        : TypeNode(start, end), name(std::move(name)), isNullable(isNullable), isImmutable(isImmutable) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct TypePath final : TypeNode {
    std::vector<TypeSegment*> segments;

    TypePath(Location start, Location end, std::vector<TypeSegment*> segments) noexcept
        : TypeNode(start, end), segments(std::move(segments)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd
