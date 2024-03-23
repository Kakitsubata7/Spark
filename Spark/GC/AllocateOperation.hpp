#pragma once

#include <list>

#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class AllocateOperation : public GCOperation {

    /* ===== Constructor ===== */

public:
    AllocateOperation(GCNode* nodePtr, std::list<GCNode*>& allNodes) : nodePtr(nodePtr),
                                                                       allNodes(allNodes) { }



    /* ===== Operation ===== */

private:
    GCNode* nodePtr;
    std::list<GCNode*>& allNodes;

public:
    bool step() override {
        allNodes.push_back(nodePtr);
        return true;
    }

};

} // Spark
