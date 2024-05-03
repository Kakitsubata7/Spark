#pragma once

#include <memory>

#include "../EvaluateVisitor.hpp"
#include "../Expressions/ExpressionNode.hpp"
#include "StatementNode.hpp"

namespace Spark {

class ExpressionStatement : public StatementNode {

    /* ===== Expression ===== */

private:
    std::unique_ptr<ExpressionNode> expr;



    /* ===== Constructors ===== */

public:
    explicit ExpressionStatement(std::unique_ptr<ExpressionNode>&& rExpr) : expr(std::move(rExpr)) { }



    /* ===== Visitor ===== */

    friend EvaluateVisitor;

public:
    bool accept(const EvaluateVisitor& visitor, Value& out) override {
        return visitor.visit(*this, out);
    }

};

} // Spark
