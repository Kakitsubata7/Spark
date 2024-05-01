#pragma once

#include <string>

#include "ExpressionNode.hpp"

namespace Spark {

class NumberExpression : ExpressionNode {

    /* ===== Data ===== */

private:
    double number;



    /* ===== Constructor ===== */

public:
    explicit NumberExpression(double number) : number(number) { }

};

} // Spark
