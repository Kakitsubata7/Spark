#pragma once

#include <forward_list>
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class Value;

class CollectOperation : public GCOperation {

    /* ===== Constructor ===== */

public:
    CollectOperation(const std::forward_list<GCNode*>& allNodes,
                     const Value const* stackBuffer, size_t stackLength) : allNodeIterator(allNodes.cbegin()) {
        
    }



    /* ===== Operation ===== */

private:
    enum class Process {
        Scanning,
        Marking,
        Sweeping
    };

    Process process = Process::Scanning;

    const std::forward_list<GCNode*>::const_iterator allNodeIterator;

    const Value const* stackBuffer;
    const size_t stackLength;

    std::queue<GCNode*> queue;
    std::unordered_set<GCNode*> visited;
    std::forward_list<GCNode*> const* currentList = nullptr;
    std::forward_list<GCNode*>::const_iterator currentIterator;

public:
    bool step() override {

        switch (process) {
            case Process::Scanning: {
                break;
            }

            case Process::Marking: {
                break;
            }

            case Process::Sweeping: {
                break;
            }
        }

        if (finishedMarking) {
            /* Sweeping */

            return true;
        }

        /* Marking */
        if (currentList != nullptr) {
            // If every node is traversed, the operation is complete
            if (queue.empty()) {
                finishedMarking = true;
                return true;
            }

            // Get the iterator of a node and start traversing it
            currentList = &(queue.front()->neighbors());
            currentIterator = currentList->cbegin();
            queue.pop();
        }

        if (currentIterator != currentList->cend()) {
            GCNode* node = *currentIterator;
            if (!node->isMarked)
                queue.push(node);
            currentIterator++;
        }

        return false;
    }

};

} // Spark
