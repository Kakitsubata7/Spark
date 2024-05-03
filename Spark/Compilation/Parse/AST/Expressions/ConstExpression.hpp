#pragma once

#include "../../Types/Value.hpp"
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

public:
    void accept(const EvaluateVisitor& visitor) override {
        visitor.visit(*this);
    }

};

} // Spark
