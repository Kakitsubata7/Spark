#include "GC.hpp"

#include <cstring>

namespace Spark {

void GC::step() {
    if (operationQueue.empty())
        return;

    const GCOperation& operation = operationQueue.front();

    operationQueue.pop();
}

void GC::collect(Value* stackBuffer, size_t stackLength) {
    // Scan the stack for entry nodes
    // Value* stack = new Value[stackLength];
    // std::memcpy(stack, stackBuffer, stackLength * sizeof(Value));
}

} // Spark
