#include "GCNode.hpp"

#include "GC.hpp"

namespace Spark {

    /* ===== Operations ===== */

    void GCNode::collect() {
        // Erase the node from GC tracking and push it as a garbage node
        gcPtr->nodeTrackingSet.erase(this);
        gcPtr->garbageNodeQueue.push(this);
    }

} // Spark
