#pragma once

#include "../Types/Value.hpp"
#include "Expressions/BinaryExpression.hpp"
#include "Expressions/ConstExpression.hpp"

namespace Spark {

class AddExpression;
class DivideExpression;
class ExpressionStatement;
class ModulusExpression;
class MultiplyExpression;
class SubtractExpression;
class Value;

class EvaluateVisitor {

    /* ===== Visit ===== */

public:
    template<typename Op>
    [[nodiscard]]
    bool visit(BinaryExpression& expr, Value& out, Op op) const {
        // Evaluate left and right expressions
        Value left, right;
        bool isLeftConst = expr.left->accept(*this, left);
        bool isRightConst = expr.right->accept(*this, right);

        // Both expressions are constant, meaning this expression can be constant as well
        if (isLeftConst && isRightConst) {
            out = op(left, right);
            return true;
        }

        // One of the nodes cannot be constant
        if (isLeftConst && !(dynamic_cast<ConstExpression*>(expr.left.get())))
            expr.left = std::make_unique<ConstExpression>(left);
        else if (isRightConst && !(dynamic_cast<ConstExpression*>(expr.right.get())))
            expr.right = std::make_unique<ConstExpression>(right);
        return false;
    }

    [[nodiscard]]
    bool visit(AddExpression& expr, Value& out) const;

    [[nodiscard]]
    bool visit(ConstExpression& expr, Value& out) const;

    [[nodiscard]]
    bool visit(DivideExpression& expr, Value& out) const;

    [[nodiscard]]
    bool visit(MultiplyExpression& expr, Value& out) const;

    [[nodiscard]]
    bool visit(ModulusExpression& expr, Value& out) const;

    [[nodiscard]]
    bool visit(SubtractExpression& expr, Value& out) const;

    [[nodiscard]]
    bool visit(ExpressionStatement& stmt, Value& out) const;

};

} // Spark
