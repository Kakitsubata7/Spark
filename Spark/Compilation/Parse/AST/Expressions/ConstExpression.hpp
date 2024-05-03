#pragma once

#include "../../Types/Value.hpp"
#include "../EvaluateVisitor.hpp"
#include "ExpressionNode.hpp"

namespace Spark {

class ConstExpression : public ExpressionNode {

    /* ===== Data ===== */

private:
    Value value;



    /* ===== Constructor ===== */

public:
    explicit ConstExpression(const Value& value) : value(value) { }
    explicit ConstExpression(Value&& value) : value(std::move(value)) { }



    /* ===== Visitor ===== */

    friend EvaluateVisitor;

public:
    bool accept(const EvaluateVisitor& visitor, Value& out) override {
        return visitor.visit(*this, out);
    }

};

} // Spark
