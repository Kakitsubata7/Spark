#pragma once

#include <unordered_map>

#include "../GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class AddEntryOperation : GCOperation {

    /* ===== Operation ===== */

private:
    std::unordered_map<GCNode*, size_t>& entryMap;
    GCNode* node;

public:
    bool step() override {
        if (entryMap.find(node) == entryMap.end())
            entryMap.insert({node, 1});
        else
            entryMap[node]++;
        return true;
    }



    /* ===== Constructor ===== */

public:
    AddEntryOperation(std::unordered_map<GCNode*, size_t>& entryMap, GCNode* node)
        : entryMap(entryMap), node(node) { }

};

} // Spark
