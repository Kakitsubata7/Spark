#pragma once

#include "BinaryExpression.hpp"

namespace Spark {

class AddExpression : public BinaryExpression {

    /* ===== Constructor ===== */

public:
    AddExpression(std::unique_ptr<ExpressionNode>& left, std::unique_ptr<ExpressionNode>& right)
        : BinaryExpression(left, right) { }



    /* ===== Visitor ===== */

public:
    void accept(const EvaluateVisitor& visitor) override {
        visitor.visit(*this);
    }

};

} // Spark
