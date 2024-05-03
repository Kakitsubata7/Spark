#pragma once

#include <memory>

#include "ExpressionNode.hpp"

namespace Spark {

class BinaryExpression : public ExpressionNode {

    /* ===== Expressions ===== */

protected:
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;



    /* ===== Constructor ===== */

public:
    BinaryExpression(std::unique_ptr<ExpressionNode>&& rLeft, std::unique_ptr<ExpressionNode>&& rRight)
        : left(std::move(rLeft)), right(std::move(rRight)) { }

};

} // Spark
