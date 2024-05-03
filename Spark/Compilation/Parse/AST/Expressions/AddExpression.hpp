#pragma once

#include "../../Types/Value.hpp"
#include "../EvaluateVisitor.hpp"
#include "BinaryExpression.hpp"

namespace Spark {

class AddExpression : public BinaryExpression {

    /* ===== Constructor ===== */

public:
    AddExpression(std::unique_ptr<ExpressionNode>& left, std::unique_ptr<ExpressionNode>& right)
        : BinaryExpression(left, right) { }



    /* ===== Visitor ===== */

    friend EvaluateVisitor;

public:
    bool accept(const EvaluateVisitor& visitor, Value& out) override {
        return visitor.visit(*this, out);
    }

};

} // Spark
