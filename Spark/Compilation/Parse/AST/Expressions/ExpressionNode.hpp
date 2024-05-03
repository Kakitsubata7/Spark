#pragma once

#include <memory>

#include "../ASTNode.hpp"

namespace Spark {

class ExpressionNode : public ASTNode {

    /* ===== Destructor ===== */

public:
    virtual ~ExpressionNode() = default;

};

} // Spark
