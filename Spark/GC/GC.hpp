#pragma once

#include <memory>
#include <queue>
#include <string>
#include <unordered_set>

#include "AllocateOperation.hpp"
#include "CollectOperation.hpp"
#include "GCNode.hpp"
#include "GCOperation.hpp"
#include "GCPtr.hpp"

namespace Spark {

class Value;

class GC {

    /* ===== Constructor & Destructor ===== */

public:
    GC() = default;
    ~GC();



    /* ===== Data ===== */

private:
    std::unordered_set<GCNode*> allNodeSet;
    std::unordered_set<GCNode*> entryNodeSet;



    /* ===== Operations ===== */

private:
    std::queue<GCOperation*> operationQueue;

public:
    void step();

    void collect() {
        // Pend the collection to the operation queue
        operationQueue.emplace(new CollectOperation(allNodeSet, entryNodeSet));
    }

    template <typename T, typename... Args>
    [[nodiscard]]
    GCPtr<T> allocate(Args&&... args) {
        // Allocate the data and the GC node
        GCNode* nodePtr = new GCNode(GCNode::make<T>(std::forward<Args>(args)...));

        // Pend the allocation to the operation queue
        operationQueue.emplace(new AllocateOperation(nodePtr, allNodeSet));

        return GCPtr<T>(nodePtr);
    }

};

} // Spark
