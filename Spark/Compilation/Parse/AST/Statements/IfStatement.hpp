#pragma once

#include "SequenceStatement.hpp"
#include "StatementNode.hpp"

namespace Spark {

class IfStatement : public StatementNode {

    /* ===== Data ===== */

private:
    std::shared_ptr<ExpressionNode> condition;
    std::shared_ptr<SequenceStatement> ifBody;
    std::shared_ptr<SequenceStatement> elseBody;



    /* ===== Constructor ===== */

public:
    IfStatement(const std::shared_ptr<ExpressionNode>& condition,
                const std::shared_ptr<SequenceStatement>& ifBody,
                const std::shared_ptr<SequenceStatement>& elseBody)
        : condition(condition), ifBody(ifBody), elseBody(elseBody) { }

};

} // Spark
