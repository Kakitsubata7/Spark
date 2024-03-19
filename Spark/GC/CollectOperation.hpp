#pragma once

#include <forward_list>
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class CollectOperation : private GCOperation {

    /* ===== Constructor ===== */

public:
    CollectOperation(const std::forward_list<GCNode*>& allNodes, const std::vector<GCNode*>& entryNodes) {
        for (GCNode* entryNode : entryNodes)
            queue.push(entryNode);
    }



    /* ===== Operation ===== */

private:
    const std::forward_list<GCNode*>::const_iterator allNodeIterator;
    std::queue<GCNode*> queue;
    std::unordered_set<GCNode*> visited;
    std::forward_list<GCNode*> const* currentList = nullptr;
    std::forward_list<GCNode*>::const_iterator currentIterator;
    bool finishedMarking = false;

public:
    bool step() override {
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
            currentList = &(queue.front()->getNeighbors());
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
