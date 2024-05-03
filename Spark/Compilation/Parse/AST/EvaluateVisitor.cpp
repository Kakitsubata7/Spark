#include "EvaluateVisitor.hpp"

#include "Expressions/AddExpression.hpp"
#include "Expressions/ConstExpression.hpp"

namespace Spark {

    void EvaluateVisitor::visit(ConstExpression&) const { }

    void EvaluateVisitor::visit(AddExpression& expr) const {

    }

} // Spark
