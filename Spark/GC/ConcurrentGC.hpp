#pragma once

#include <memory>

#include "../Utilities/ConcurrentQueue.hpp"
#include "GCBase.hpp"
#include "Operations/GCOperation.hpp"

#include <array>
#include <atomic>
#include <stddef.h>
#include <thread>

namespace Spark {

class ConcurrentGC: public GCBase {

    /* ===== Thread ===== */

private:
    std::thread thread;
    std::atomic<bool> canRun;



    /* ===== Constructor & Destructor ===== */

public:
    ConcurrentGC();
    ~ConcurrentGC();



    /* ===== Operations ===== */

private:
    constexpr static const size_t CACHE_SIZE = 10;
    std::array<std::unique_ptr<GCOperation>, CACHE_SIZE> cachedOperations;
    size_t currentCacheSize = 0;

    ConcurrentQueue<std::unique_ptr<GCOperation>> operationQueue;

public:
    void addEntryNode(GCNode* node) override;
    void removeEntryNode(GCNode* node) override;

    void step() override;

};

} // Spark
