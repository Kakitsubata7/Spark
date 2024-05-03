#include "ConstExpression.hpp"

#include "../EvaluateVisitor.hpp"

namespace Spark {

    /* ===== Visitor ===== */

    bool ConstExpression::accept(const EvaluateVisitor& visitor, Value& out)  {
        return visitor.visit(*this, out);
    }

} // Spark
