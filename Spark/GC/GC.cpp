#include "GC.hpp"

#include <cstring>

namespace Spark {

    /* ===== Destructor ===== */

    GC::~GC() {
        for (GCNode* nodePtr : allNodeList)
            delete nodePtr;
    }



    /* ===== Operations ===== */

    static std::unique_ptr<GCOperation> currentOperation;

    void GC::step() {
        if (operationQueue.empty())
            return;

        std::unique_ptr<GCOperation> operation = operationQueue.front();
        operation->step();
        operationQueue.pop();
    }

    void GC::collect(Value* stackBuffer, size_t stackLength) {
        // Scan the stack to find entry nodes
        // Value* stack = new Value[stackLength];
        // std::memcpy(stack, stackBuffer, stackLength * sizeof(Value));
    }

} // Spark
