#pragma once

namespace Spark {

class EvaluateVisitor;
class Value;

class ASTNode {

    /* ===== Destructor ===== */

public:
    ~ASTNode() = default;



    /* ===== Visitor ===== */

public:
    [[nodiscard]]
    virtual bool accept(const EvaluateVisitor& visitor, Value& out) = 0;

};

} // Spark
