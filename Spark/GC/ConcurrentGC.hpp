#pragma once

#include <memory>

#include "../Utilities/ConcurrentQueue.hpp"
#include "GCBase.hpp"
#include "GCOperation.hpp"

namespace Spark {

class ConcurrentGC: public GCBase {

    /* ===== Operations ===== */

private:
    ConcurrentQueue<std::unique_ptr<GCOperation>> operationQueue;

public:
    void addEntryNode(GCNode* node) override;
    void removeEntryNode(GCNode* node) override;

};

} // Spark
