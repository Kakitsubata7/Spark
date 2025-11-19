#pragma once

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

} // Spark::FrontEnd::AST
