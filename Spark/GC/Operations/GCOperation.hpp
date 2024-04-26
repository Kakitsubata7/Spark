#pragma once

namespace Spark {

class GCOperation {

    /* ===== Operation ===== */

public:
    virtual bool step() = 0;



    /* ===== Destructor ===== */

public:
    virtual ~GCOperation() = default;

};

} // Spark
