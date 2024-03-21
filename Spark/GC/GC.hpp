#pragma once

#include <memory>
#include <queue>
#include <string>
#include <unordered_set>

#include "../Types/Value.hpp"
#include "AllocateOperation.hpp"
#include "GCNode.hpp"
#include "GCOperation.hpp"
#include "GCPtr.hpp"

namespace Spark {

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
    std::queue<std::unique_ptr<GCOperation>> operationQueue;

public:
    void step();

    void collect(Value* stackBuffer, size_t stackLength);

    template <typename T, typename... Args>
    [[nodiscard]]
    GCPtr<T> allocate(Args&&... args) {
        // Allocate the data and the GC node
        T* dataPtr = new T(std::forward<Args>(args)...);
        void (*destructorPtr)(void*) = [](void* obj) { static_cast<T*>(obj)->~T(); };
        GCNode* nodePtr = new GCNode(this, dataPtr, destructorPtr);

        // Pend the allocation to the operation queue
        operationQueue.emplace(std::make_unique<AllocateOperation>(nodePtr, allNodeSet));

        return GCPtr<T>(nodePtr);
    }

};

} // Spark
