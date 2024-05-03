#pragma once

namespace Spark {

class EvaluateVisitor;

class ASTNode {

    /* ===== Destructor ===== */

public:
    ~ASTNode() = default;



    /* ===== Visitor ===== */

public:
    virtual void accept(const EvaluateVisitor& visitor) = 0;

};

} // Spark
