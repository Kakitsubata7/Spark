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
        : allNodeList(allNodeList),
          allNodeIterator(allNodeList.cbegin()),
          stackLength(stackLength),
          process(Process::Scanning) {
        this->stackBuffer = new Value[stackLength];
        std::memcpy(this->stackBuffer, stackBuffer, stackLength);
    }

    CollectOperation(std::list<GCNode*>& allNodeList, const std::vector<GCNode*>& entryNodes)
        : allNodeList(allNodeList),
          allNodeIterator(allNodeList.cbegin()),
          stackBuffer(nullptr),
          stackLength(0),
          process(Process::Preprocessing) {
        for (GCNode* node : entryNodes)
            queue.push(node);
    }



    /* ===== Operation ===== */

private:
    enum class Process {
        Scanning,       // Scan the stack for entry point nodes
        Preprocessing,  // Set every node as unmarked
        Marking,        // Mark reachable nodes
        Sweeping        // Deallocate unreachable nodes
    };

    Process process;

    /* Scanning Fields */
    Value* stackBuffer;
    size_t stackIndex = 0;
    const size_t stackLength;

    /* Preprocessing & Sweeping Fields */
    std::list<GCNode*>& allNodeList;
    std::list<GCNode*>::const_iterator allNodeIterator;

    /* Marking Fields */
    std::queue<GCNode*> queue;
    std::list<GCNode*> const* currentList = nullptr;
    std::list<GCNode*>::const_iterator currentIterator;

public:
    bool step() override {
        switch (process) {
            case Process::Scanning: {

                Value* valuePtr = stackBuffer + stackIndex;
                GCNode* node;
                if (valuePtr->tryGetGCNode(node)) {
                    // Push the GC node pointer to the queue
                    queue.push(node);
                }
                stackIndex++;

                if (stackIndex == stackLength)
                    process = Process::Preprocessing;

                return false;
            }

            case Process::Preprocessing: {

                if (allNodeIterator != allNodeList.cend()) {
                    GCNode* node = *allNodeIterator;
                    node->isMarked = false;
                    allNodeIterator++;
                } else
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
                    if (!node->isMarked || node->referenceCount == 0) {
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
