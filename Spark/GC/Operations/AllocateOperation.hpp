#pragma once

#include <unordered_set>

#include "../GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class AllocateOperation : public GCOperation {

    /* ===== Constructor ===== */

public:
    AllocateOperation(GCNode* node, std::unordered_set<GCNode*>& allNodeSet) : node(node),
                                                                               allNodeSet(allNodeSet) { }



    /* ===== Operation ===== */

private:
    GCNode* node;
    std::unordered_set<GCNode*>& allNodeSet;

public:
    bool step() override {
        allNodeSet.insert(node);
        return true;
    }

};

} // Spark
