#pragma once

#include <queue>
#include <unordered_set>

#include "GCPtr.hpp"
#include "GCNode.hpp"

namespace Spark {

class GC {

    /* ===== Friend Class ===== */

    friend class GCNode;



    /* ===== Data ===== */

private:
    std::unordered_set<GCNode*> nodeTrackingSet;

    std::queue<GCNode*> garbageNodeQueue;



    /* ===== Constructor ===== */

public:
    GC() = default;



    /* ===== Destructor ===== */

public:
    ~GC() {
        // Delete all tracking nodes
        for (GCNode* nodePtr : nodeTrackingSet)
            delete nodePtr;

        // Delete all garbage nodes
        while (!garbageNodeQueue.empty()) {
            GCNode* nodePtr = garbageNodeQueue.front();
            garbageNodeQueue.pop();
            delete nodePtr;
        }
    }



    /* ===== Factory Methods ===== */

public:
    template <typename T, typename... Args>
    [[nodiscard]]
    GCPtr<T> make(Args&&... args) {
        T* dataPtr = new T(std::forward<Args>(args)...);
        void (*destructorPtr)(void*) = [](void* obj) { static_cast<T*>(obj)->~T(); };
        GCNode* nodePtr = new GCNode(1, this, dataPtr, destructorPtr);
        nodeTrackingSet.insert(nodePtr);
        return GCPtr<T>(nodePtr);
    }

};

} // Spark
