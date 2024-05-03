#pragma once

#include <memory>
#include <vector>

#include "../Expressions/ExpressionNode.hpp"
#include "StatementNode.hpp"

namespace Spark {

class SequenceStatement : public StatementNode {

    /* ===== Expressions ===== */

private:
    std::vector<std::shared_ptr<ExpressionNode>> expressions;

};

} // Spark
