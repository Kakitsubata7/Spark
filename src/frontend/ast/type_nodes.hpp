#pragma once

#include <string>
#include <vector>

#include "node.hpp"

namespace Spark::FrontEnd::AST {

struct TypeNode : Node {
    TypeNode(size_t line, size_t column) noexcept : Node(line, column) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct TypeSegment final : TypeNode {
    std::string name;
    bool isNullable;
    bool isImmutable;

    TypeSegment(size_t line, size_t column, std::string name, bool isImmutable, bool isNullable) noexcept
        : TypeNode(line, column), name(std::move(name)), isNullable(isNullable), isImmutable(isImmutable) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct PathType final : TypeNode {
    std::vector<TypeSegment*> segments;

    PathType(size_t line, size_t column, std::vector<TypeSegment*> segments) noexcept
        : TypeNode(line, column), segments(std::move(segments)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd::AST
