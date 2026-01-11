#pragma once

namespace Spark::FrontEnd {

struct Node;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(Node& node) { }
};

} // Spark::FrontEnd
