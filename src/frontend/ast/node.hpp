#pragma once

#include "node_visitor.hpp"

namespace Spark::FrontEnd::AST {

class NodeVisitor;

struct Node {
    size_t line = 0;
    size_t column = 0;

    virtual ~Node() = default;

    virtual void accept(NodeVisitor& v) = 0;
};

} // Spark::FrontEnd::AST
