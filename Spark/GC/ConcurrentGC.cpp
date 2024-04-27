#include "ConcurrentGC.hpp"

namespace Spark {

    /* ===== Constructor ===== */

    ConcurrentGC::ConcurrentGC() : canRun(true) {
        thread = std::thread([&]() -> void {
            while (canRun.load()) {
                std::unique_ptr<GCOperation> operation = operationQueue.waitUntilPop();
                while (!operation->step()) { }
            }
        });
    }

    ConcurrentGC::~ConcurrentGC() {
        canRun.store(false);
        if (thread.joinable())
            thread.join();
    }



    /* ===== Operations ===== */

    void ConcurrentGC::addEntryNode(GCNode* node) {

    }

    void ConcurrentGC::removeEntryNode(GCNode* node) {

    }

    void ConcurrentGC::step() {
        if (currentCacheSize == CACHE_SIZE) {
            operationQueue.moveArray(cachedOperations);
            currentCacheSize = 0;
        }
    }

} // Spark
