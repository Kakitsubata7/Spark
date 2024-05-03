#pragma once

#include "BinaryExpression.hpp"
#include "../EvaluateVisitor.hpp"

namespace Spark {

class AddExpression : public BinaryExpression {

    /* ===== Constructor ===== */

public:
    AddExpression(std::unique_ptr<ExpressionNode>& left, std::unique_ptr<ExpressionNode>& right)
        : BinaryExpression(left, right) { }



    /* ===== Visitor ===== */

    friend EvaluateVisitor;

public:
    void accept(const EvaluateVisitor& visitor) override {
        visitor.visit(*this);
    }

};

} // Spark
