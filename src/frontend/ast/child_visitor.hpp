#pragma once

#include "node_visitor.hpp"

namespace Spark::FrontEnd {

class ChildVisitor : public NodeVisitor {
public:
    void visit(Node& node) override;
};

} // Spark::FrontEnd
