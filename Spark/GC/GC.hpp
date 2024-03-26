#pragma once

#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "AllocateOperation.hpp"
#include "CollectOperation.hpp"
#include "GCNode.hpp"
#include "GCOperation.hpp"

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
//    std::unordered_set<GCNode*> youngNodeSet;
//    std::unordered_set<GCNode*> survivorNodeSet;
//    std::unordered_set<GCNode*> oldNodeSet;
    std::unordered_map<GCNode*, long> entryNodeMap;



    /* ===== Operations ===== */

private:
    std::queue<GCOperation*> operationQueue;

public:
    void step();

    void collect() {
        // Pend the collection to the operation queue
        operationQueue.emplace(new CollectOperation(allNodeSet, entryNodeMap));
    }

    template <typename T, typename... Args>
    [[nodiscard]]
    GCNode* allocate(Args&&... args) {
        // Allocate the data and the GC node
        GCNode* nodePtr = new GCNode(GCNode::make<T>(std::forward<Args>(args)...));

        // Pend the allocation to the operation queue
        operationQueue.emplace(new AllocateOperation(nodePtr, allNodeSet));

        return nodePtr;
    }

    void registerEntryNode(GCNode* node) {
        if (entryNodeMap.find(node) != entryNodeMap.end())
            entryNodeMap[node]++;
        else
            entryNodeMap.insert({node, 1});
    }

    void unregisterEntryNode(GCNode* node) {
        if (entryNodeMap.find(node) == entryNodeMap.end())
            throw std::runtime_error("GC node is not registered.");

        // Erase if node is completely unregistered
        entryNodeMap[node]--;
        if (entryNodeMap[node] == 0)
            entryNodeMap.erase(node);
    }

};

} // Spark
