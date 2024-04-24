#include "CollectOperation.hpp"

#include <iostream>

namespace Spark {

    bool CollectOperation::step() {
        switch (process) {
            case Process::Entry: {

                if (entryNodeIterator != entryNodeMap.cend()) {
                    std::pair<GCNode*, long> pair = *entryNodeIterator;
                    GCNode* node = pair.first;
                    queue.push(node);
                    entryNodeIterator++;
                } else {
                    process = Process::Preprocessing;
                    allNodeIterator = allNodeSet.cbegin();
                }

                return false;
            }

            case Process::Preprocessing: {

                if (allNodeIterator != allNodeSet.cend()) {
                    GCNode* node = *allNodeIterator;

                    // Deallocate node with no reference count. Entry nodes are ignored
                    if (node->referenceCount == 0 && entryNodeMap.find(node) == entryNodeMap.cend()) {
                        delete node;
                        allNodeIterator = allNodeSet.erase(allNodeIterator);
                    } else {
                        node->isMarked = false;
                        allNodeIterator++;
                    }
                } else
                    process = Process::Marking;

                return false;
            }

            case Process::Marking: {

                if (currentList == nullptr) {
                    // If every node is traversed, marking is complete
                    if (queue.empty()) {
                        process = Process::Sweeping;
                        allNodeIterator = allNodeSet.cbegin(); // Reset 'allNodeIterator' to the beginning
                        return false;
                    }

                    // Get the iterator of a node and start traversing it
                    GCNode* node = queue.front();
                    queue.pop();
                    node->isMarked = true;
                    currentList = &(node->neighbors());
                    currentIterator = currentList->cbegin();
                }

                // Traverse the neighbors of the current node
                if (currentIterator != currentList->cend()) {
                    GCNode* node = *currentIterator;
                    if (!node->isMarked)
                        queue.push(node);
                    currentIterator++;
                } else
                    currentList = nullptr;

                return false;
            }

            case Process::Sweeping: {

                if (allNodeIterator != allNodeSet.cend()) {
                    GCNode* node = *allNodeIterator;

                    // Deallocate unmarked GC node
                    if (!node->isMarked) {
                        delete node;
                        allNodeIterator = allNodeSet.erase(allNodeIterator);
                    } else
                        allNodeIterator++;

                    return false;
                }

                return true;
            }
        }
    }

} // Spark
