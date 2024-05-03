#include "EvaluateVisitor.hpp"

#include "../Types/Value.hpp"
#include "Expressions/AddExpression.hpp"
#include "Expressions/ConstExpression.hpp"

namespace Spark {

    bool EvaluateVisitor::visit(AddExpression& expr, Value& out) const {
        // Evaluate left and right expressions
        Value left, right;
        bool leftIsConst = expr.left->accept(*this, left);
        bool rightIsConst = expr.right->accept(*this, right);

        // Both expressions are constant, meaning this expression can be constant as well
        if (leftIsConst && rightIsConst) {
            out = left + right;
            return true;
        }

        // One of the nodes cannot be constant
        if (leftIsConst)
            expr.left = std::make_unique<ConstExpression>(left);
        else if (rightIsConst)
            expr.right = std::make_unique<ConstExpression>(right);
        return false;
    }

    bool EvaluateVisitor::visit(ConstExpression& expr, Value& out) const {
        out = expr.value;
        return true;
    }

} // Spark
