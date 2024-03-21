#pragma once

#include <list>

#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class AllocateOperation : public GCOperation {

    /* ===== Constructor ===== */

public:
    AllocateOperation(GCNode* nodePtr, std::unordered_set<GCNode*>& allNodeSet) : nodePtr(nodePtr),
                                                                                  allNodeSet(allNodeSet) { }



    /* ===== Operation ===== */

private:
    GCNode* nodePtr;
    std::unordered_set<GCNode*>& allNodeSet;

public:
    bool step() override {
        allNodeSet.insert(nodePtr);
        return true;
    }

};

} // Spark
