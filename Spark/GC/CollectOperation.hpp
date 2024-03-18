#pragma once

#include <unordered_set>

#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class CollectOperation : public GCOperation {

    /* ===== Constructor ===== */

public:
    CollectOperation() {

    }



    /* ===== Operation ===== */

private:
    std::unordered_set<GCNode*> visited;

public:
    bool step() override {



        return true;
    }

};

} // Spark
