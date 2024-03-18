#pragma once

#include <queue>
#include <string>

#include "GCOperation.hpp"

namespace Spark {

class GC {

    /* ===== Constructor & Destructor ===== */

public:
    GC() = default;
    ~GC() = default;



    /* ===== Operation ===== */

private:
    std::queue<GCOperation> operationQueue;

    void step();

};

} // Spark
