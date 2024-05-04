#include "EvaluateVisitor.hpp"

#include "../Types/Value.hpp"
#include "Expressions/AddExpression.hpp"
#include "Expressions/ConstExpression.hpp"
#include "Expressions/DivideExpression.hpp"
#include "Expressions/ModulusExpression.hpp"
#include "Expressions/MultiplyExpression.hpp"
#include "Expressions/SubtractExpression.hpp"
#include "Statements/ExpressionStatement.hpp"

namespace Spark {

    bool EvaluateVisitor::visit(AddExpression& expr, Value& out) const {
        return visit(expr, out,
                     [](const Value& left, const Value& right) -> Value { return left + right; });
    }

    bool EvaluateVisitor::visit(ConstExpression& expr, Value& out) const {
        out = expr.value;
        return true;
    }

    bool EvaluateVisitor::visit(DivideExpression& expr, Value& out) const {
        return visit(expr, out,
                     [](const Value& left, const Value& right) -> Value { return left / right; });
    }

    bool EvaluateVisitor::visit(ModulusExpression& expr, Value& out) const {
        return visit(expr, out,
                     [](const Value& left, const Value& right) -> Value { return left % right; });
    }

    bool EvaluateVisitor::visit(MultiplyExpression& expr, Value& out) const {
        return visit(expr, out,
                     [](const Value& left, const Value& right) -> Value { return left * right; });
    }

    bool EvaluateVisitor::visit(SubtractExpression& expr, Value& out) const {
        return visit(expr, out,
                     [](const Value& left, const Value& right) -> Value { return left - right; });
    }

    bool EvaluateVisitor::visit(ExpressionStatement& stmt, Value& out) const {
        // Evaluate the expression
        // If it can be constant, substitute it to be a constant expression
        // Return if the statement can be simplified to a statement with a single constant expression
        Value value;
        if (stmt.expr->accept(*this, value)) {
            out = value;
            stmt.expr = std::make_unique<ConstExpression>(value);
            return true;
        }
        return false;
    }

} // Spark
