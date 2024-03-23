#include "GC.hpp"

namespace Spark {

    /* ===== Destructor ===== */

    GC::~GC() {
        // Deallocate all operations
        while (!operationQueue.empty()) {
            GCOperation* operation = operationQueue.front();
            delete operation;
            operationQueue.pop();
        }

        // Deallocate all GC nodes
        for (GCNode* node : allNodeList)
            delete node;
    }



    /* ===== Operations ===== */

    static GCOperation* currentOperation = nullptr;

    void GC::step() {
        // Execute current operation until finished
        if (currentOperation != nullptr) {
            if (currentOperation->step()) {
                delete currentOperation;
                currentOperation = nullptr;
            }
            return;
        }

        // No queued operation
        if (operationQueue.empty())
            return;

        // Get a new operation and execute one step
        currentOperation = operationQueue.front();
        if (currentOperation->step()) {
            delete currentOperation;
            currentOperation = nullptr;
        }
        operationQueue.pop();
    }

} // Spark
