#pragma once

#include <memory>
#include <queue>
#include <unordered_set>

#include "GCBase.hpp"
#include "Operations/GCOperation.hpp"

namespace Spark {

class SingleThreadedGC : public GCBase {

    /* ===== Heap ===== */

private:
    std::unordered_set<GCNode*> heap;



    /* ===== Operations ===== */

private:
    std::queue<std::unique_ptr<GCOperation>> operationQueue;

public:
    void addEntryNode(GCNode* node) override;
    void removeEntryNode(GCNode* node) override;

    void step() override;

};

} // Spark
