#pragma once

namespace Spark {

class GCOperation {

    /* ===== Interface Methods ===== */

public:
    virtual bool step() = 0;



    /* ===== Destructor ===== */

public:
    virtual ~GCOperation() = default;

};

} // Spark
