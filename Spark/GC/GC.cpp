#include "GC.hpp"

namespace Spark {

void GC::step() {
    if (operationQueue.empty())
        return;

    const GCOperation& operation = operationQueue.front();

    operationQueue.pop();
}

} // Spark
