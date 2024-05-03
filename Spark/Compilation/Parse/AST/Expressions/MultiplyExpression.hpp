#pragma once

#include "../../Types/Value.hpp"
#include "../EvaluateVisitor.hpp"
#include "BinaryExpression.hpp"

namespace Spark {

class MultiplyExpression : public BinaryExpression {

    /* ===== Constructor ===== */

public:
    MultiplyExpression(std::unique_ptr<ExpressionNode>&& rLeft, std::unique_ptr<ExpressionNode>&& rRight)
        : BinaryExpression(std::move(rLeft), std::move(rRight)) { }



    /* ===== Visitor ===== */

    friend EvaluateVisitor;

public:
    bool accept(const EvaluateVisitor& visitor, Value& out) override {
        return visitor.visit(*this, out);
    }

};

} // Spark
