#pragma once

#include <memory>

#include "ExpressionNode.hpp"

namespace Spark {

class BinaryExpression : public ExpressionNode {

    /* ===== Data ===== */

private:
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;



    /* ===== Constructor ===== */

public:
    BinaryExpression(std::unique_ptr<ExpressionNode>& left, std::unique_ptr<ExpressionNode>& right)
        : left(std::move(left)), right(std::move(right)) { }

};

} // Spark
