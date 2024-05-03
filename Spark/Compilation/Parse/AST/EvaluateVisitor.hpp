#pragma once

namespace Spark {

class AddExpression;
class ConstExpression;

class EvaluateVisitor {

    /* ===== Visit ===== */

public:
    void visit(AddExpression& expr) const;
    void visit(ConstExpression& expr) const;

};

} // Spark
