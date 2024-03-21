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

    /* ===== Constructor ===== */

public:
    CollectOperation(const std::list<GCNode*>& allNodes,
                     const Value* stackBuffer,
                     size_t stackLength) : allNodeIterator(allNodes.cbegin()), stackLength(stackLength) {
        this->stackBuffer = new Value[stackLength];
        std::memcpy(this->stackBuffer, stackBuffer, stackLength);
    }



    /* ===== Operation ===== */

private:
    enum class Process {
        Preprocessing,  // Set every node as unmarked
        Scanning,       // Scan the stack for entry point nodes
        Marking,        // Mark reachable nodes
        Sweeping        // Deallocate unreachable nodes
    };

    Process process = Process::Scanning;

    const std::list<GCNode*>::const_iterator allNodeIterator;

    Value* stackBuffer;
    size_t stackIndex = 0;
    const size_t stackLength;

    std::queue<GCNode*> queue;
    std::unordered_set<GCNode*> visited;
    std::list<GCNode*> const* currentList = nullptr;
    std::list<GCNode*>::const_iterator currentIterator;

public:
    bool step() override {

        switch (process) {
            case Process::Preprocessing: {



                return false;
            }

            case Process::Scanning: {

                if ((stackBuffer + stackIndex)->isReferenceType()) {
                    // TODO: Push the GC node pointer to the queue
                }
                stackIndex++;

                if (stackIndex == stackLength)
                    process = Process::Marking;

                return false;
            }

            case Process::Marking: {

                if (queue.empty()) {
                    process = Process::Sweeping;
                    return false;
                }



                return false;
            }

            case Process::Sweeping: {



                return false;
            }
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
