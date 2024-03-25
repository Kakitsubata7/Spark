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



    /* ===== Rules ===== */

private:
    long updateInterval = 5;
    long stepPerUpdate = 1;



    /* ===== Data ===== */

private:
    std::unordered_set<GCNode*> allNodeSet;



    /* ===== Operations ===== */

private:
    std::queue<GCOperation*> operationQueue;

public:
    void step();

    void collect(Value* stackBuffer, size_t stackLength) {
        operationQueue.emplace(new CollectOperation(allNodeSet, stackBuffer, stackLength));
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
