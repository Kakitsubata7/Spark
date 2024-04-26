#pragma once

#include <cstring>
#include <list>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../Types/Value.hpp"
#include "../GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class CollectOperation : public GCOperation {

    /* ===== Constructor & Destructor ===== */

public:
    CollectOperation(std::unordered_set<GCNode*>& allNodeSet, const std::unordered_map<GCNode*, long>& entryNodeMap)
        : allNodeSet(allNodeSet),
          entryNodeMap(entryNodeMap),
          entryNodeIterator(entryNodeMap.cbegin()),
          process(Process::Entry) { }



    /* ===== Operation ===== */

private:
    enum class Process {
        Entry,          // Pushing entry point nodes to the queue
        Preprocessing,  // Set every node as unmarked, and deallocate nodes with no reference count
        Marking,        // Mark reachable nodes
        Sweeping        // Deallocate unreachable nodes
    };

    Process process;

    /* Entry */
    const std::unordered_map<GCNode*, long>& entryNodeMap;
    std::unordered_map<GCNode*, long>::const_iterator entryNodeIterator;

    /* Preprocessing & Sweeping Fields */
    std::unordered_set<GCNode*>& allNodeSet;
    std::unordered_set<GCNode*>::const_iterator allNodeIterator;

    /* Marking Fields */
    std::queue<GCNode*> queue;
    std::list<GCNode*> const* currentList = nullptr;
    std::list<GCNode*>::const_iterator currentIterator;

public:
    bool step() override;

};

} // Spark
