#pragma once

namespace Spark {

class AddExpression;
class ConstExpression;
class Value;

class EvaluateVisitor {

    /* ===== Visit ===== */

public:
    [[nodiscard]]
    bool visit(AddExpression& expr, Value& out) const;

    [[nodiscard]]
    bool visit(ConstExpression& expr, Value& out) const;

};

} // Spark
