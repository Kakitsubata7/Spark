#pragma once

#include <cstring>
#include <list>
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

#include "../Types/Value.hpp"
#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class CollectOperation : public GCOperation {

    /* ===== Constructor & Destructor ===== */

public:
    CollectOperation(std::unordered_set<GCNode*>& allNodeSet, const Value* stackBuffer, size_t stackLength)
        : allNodeSet(allNodeSet),
          allNodeIterator(allNodeSet.cbegin()),
          stackLength(stackLength),
          process(Process::Scanning) {
        this->stackBuffer = new Value[stackLength];
        std::memcpy(this->stackBuffer, stackBuffer, stackLength);
    }

    CollectOperation(std::unordered_set<GCNode*>& allNodeSet, const std::vector<GCNode*>& entryNodes)
        : allNodeSet(allNodeSet),
          allNodeIterator(allNodeSet.cbegin()),
          stackBuffer(nullptr),
          stackLength(0),
          process(Process::Preprocessing) {
        for (GCNode* node : entryNodes)
            queue.push(node);
    }

    ~CollectOperation() override {
        delete stackBuffer;
    }



    /* ===== Operation ===== */

private:
    enum class Process {
        Scanning,       // Scan the stack for entry point nodes
        Preprocessing,  // Set every node as unmarked, and deallocate nodes with no reference count
        Marking,        // Mark reachable nodes
        Sweeping        // Deallocate unreachable nodes
    };

    Process process;

    /* Scanning Fields */
    Value* stackBuffer;
    size_t stackIndex = 0;
    const size_t stackLength;

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
