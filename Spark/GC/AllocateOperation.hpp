#pragma once

#include <list>

#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class AllocateOperation : public GCOperation {

    /* ===== Constructor ===== */

public:
    AllocateOperation(GCNode* nodePtr, std::list<GCNode*>& allNodeList) : nodePtr(nodePtr),
                                                                          allNodeList(allNodeList) { }



    /* ===== Operation ===== */

private:
    GCNode* nodePtr;
    std::list<GCNode*>& allNodeList;

public:
    bool step() override {
        allNodeList.push_back(nodePtr);
        return true;
    }

};

} // Spark
