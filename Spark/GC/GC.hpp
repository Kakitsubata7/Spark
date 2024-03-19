#pragma once

#include <forward_list>
#include <queue>
#include <string>

#include "../Types/Value.hpp"
#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class GC {

    /* ===== Constructor & Destructor ===== */

public:
    GC() = default;
    ~GC() = default;



    /* ===== Data ===== */

private:
    std::forward_list<GCNode*> allNodes;



    /* ===== Operation ===== */

private:
    std::queue<GCOperation> operationQueue;

    void step();

public:
    void collect(Value* stackBuffer, size_t stackLength);

};

} // Spark
