#pragma once

namespace Spark {

class ASTNode {

    /* ===== Operation ===== */

public:
    virtual void tryEvaluate() = 0;

};

} // Spark
