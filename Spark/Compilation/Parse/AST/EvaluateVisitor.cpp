#include "EvaluateVisitor.hpp"

#include "../Types/Value.hpp"
#include "Expressions/AddExpression.hpp"
#include "Expressions/ConstExpression.hpp"
#include "Expressions/SubtractExpression.hpp"
#include "Statements/ExpressionStatement.hpp"

namespace Spark {

    bool EvaluateVisitor::visit(AddExpression& expr, Value& out) const {
        // Evaluate left and right expressions
        Value left, right;
        bool isLeftConst = expr.left->accept(*this, left);
        bool isRightConst = expr.right->accept(*this, right);

        // Both expressions are constant, meaning this expression can be constant as well
        if (isLeftConst && isRightConst) {
            out = left + right;
            return true;
        }

        // One of the nodes cannot be constant
        if (isLeftConst && !(dynamic_cast<ConstExpression*>(expr.left.get())))
            expr.left = std::make_unique<ConstExpression>(left);
        else if (isRightConst && !(dynamic_cast<ConstExpression*>(expr.right.get())))
            expr.right = std::make_unique<ConstExpression>(right);
        return false;
    }

    bool EvaluateVisitor::visit(ConstExpression& expr, Value& out) const {
        out = expr.value;
        return true;
    }

    bool EvaluateVisitor::visit(SubtractExpression& expr, Value& out) const {
        // Evaluate left and right expressions
        Value left, right;
        bool isLeftConst = expr.left->accept(*this, left);
        bool isRightConst = expr.right->accept(*this, right);

        // Both expressions are constant, meaning this expression can be constant as well
        if (isLeftConst && isRightConst) {
            out = left - right;
            return true;
        }

        // One of the nodes cannot be constant
        if (isLeftConst && !(dynamic_cast<ConstExpression*>(expr.left.get())))
            expr.left = std::make_unique<ConstExpression>(left);
        else if (isRightConst && !(dynamic_cast<ConstExpression*>(expr.right.get())))
            expr.right = std::make_unique<ConstExpression>(right);
        return false;
    }

    bool EvaluateVisitor::visit(ExpressionStatement& stmt, Value& out) const {
        // Evaluate the expression
        // If it can be constant, substitute it to be a constant expression
        Value value;
        if (stmt.expr->accept(*this, value)) {
            out = value;
            stmt.expr = std::make_unique<ConstExpression>(value);
            return true;
        }
        return false;
    }

} // Spark
