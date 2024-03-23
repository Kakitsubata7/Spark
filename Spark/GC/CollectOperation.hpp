#pragma once

#include <cstring>
#include <list>
#include <memory>
#include <queue>
#include <vector>

#include "../Types/Value.hpp"
#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class CollectOperation : public GCOperation {

    /* ===== Constructor ===== */

public:
    CollectOperation(std::list<GCNode*>& allNodeList, const Value* stackBuffer, size_t stackLength)
            : allNodeList(allNodeList), allNodeIterator(allNodeList.cbegin()), stackLength(stackLength) {
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

    /* Preprocessing & Sweeping Fields */
    std::list<GCNode*>& allNodeList;
    std::list<GCNode*>::const_iterator allNodeIterator;

    /* Scanning Fields */
    Value* stackBuffer;
    size_t stackIndex = 0;
    const size_t stackLength;

    /* Marking Fields */
    std::queue<GCNode*> queue;
    std::list<GCNode*> const* currentList = nullptr;
    std::list<GCNode*>::const_iterator currentIterator;

public:
    bool step() override {

        switch (process) {
            case Process::Preprocessing: {

                if (allNodeIterator != allNodeList.cend()) {
                    GCNode* node = *allNodeIterator;
                    node->isMarked = false;
                    allNodeIterator++;
                }

                return false;
            }

            case Process::Scanning: {

                Value* valuePtr = stackBuffer + stackIndex;
                GCNode* node;
                if (valuePtr->tryGetGCNode(node)) {
                    // Push the GC node pointer to the queue
                    queue.push(node);
                }
                stackIndex++;

                if (stackIndex == stackLength)
                    process = Process::Marking;

                return false;
            }

            case Process::Marking: {

                if (currentList != nullptr) {
                    // If every node is traversed, marking is complete
                    if (queue.empty()) {
                        process = Process::Sweeping;
                        allNodeIterator = allNodeList.cbegin(); // Reset 'allNodeIterator' to the beginning
                        return false;
                    }

                    // Get the iterator of a node and start traversing it
                    currentList = &(queue.front()->neighbors());
                    currentIterator = currentList->cbegin();
                    queue.pop();
                }

                // Traverse the neighbors of the current node
                if (currentIterator != currentList->cend()) {
                    GCNode* node = *currentIterator;
                    if (!node->isMarked) {
                        node->isMarked = true;
                        queue.push(node);
                    }
                    currentIterator++;
                }

                return false;
            }

            case Process::Sweeping: {

                if (allNodeIterator != allNodeList.cend()) {
                    GCNode* node = *allNodeIterator;

                    // Delete and erase the GC node
                    if (!node->isMarked || node->referenceCount() == 0) {
                        delete node;
                        allNodeIterator = allNodeList.erase(allNodeIterator);
                    }

                    allNodeIterator++;
                    return false;
                }

                return true;
            }
        }
    }

};

} // Spark
