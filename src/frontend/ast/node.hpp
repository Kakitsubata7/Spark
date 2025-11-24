#pragma once

#include "node_visitor.hpp"
#include "utils/location.hpp"

namespace Spark::FrontEnd {

class NodeVisitor;

struct Node {
    Location start;
    Location end;

    Node(Location start, Location end) noexcept : start(start), end(end) { }

    virtual ~Node() = default;

    virtual void accept(NodeVisitor& v) = 0;
};

} // Spark::FrontEnd
