#pragma once

#include <unordered_map>

#include "../GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class RemoveEntryOperation : GCOperation {

    /* ===== Operation ===== */

private:
    std::unordered_map<GCNode*, size_t>& entryMap;
    GCNode* node;

public:
    bool step() override {
        entryMap.erase(node);
        return true;
    }



    /* ===== Constructor ===== */

public:
    RemoveEntryOperation(std::unordered_map<GCNode*, size_t>& entryMap, GCNode* node)
        : entryMap(entryMap), node(node) { }

};

} // Spark
