#pragma once

#include <memory>

namespace Spark::FrontEnd::AST {

class NodeVisitor;

class NodeBase {
public:
    virtual ~NodeBase() = default;

    virtual void accept(NodeVisitor& v) = 0;
};

using NodePtr = std::unique_ptr<NodeBase>;

} // Spark::FrontEnd::AST
