#pragma once

#include <string>

#include "node_visitor.hpp"

namespace Spark::FrontEnd::AST {

class NodeVisitor;

struct Node {
    size_t line;
    size_t column;

    Node(size_t line, size_t column) noexcept : line(line), column(column) { }

    virtual ~Node() = default;

    virtual void accept(NodeVisitor& v) = 0;
};

struct IdentifierNode final : Node {
    std::string name;

    IdentifierNode(size_t line, size_t column, std::string name) noexcept
        : Node(line, column), name(std::move(name)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd::AST
