#pragma once

#include <queue>
#include <unordered_set>
#include <vector>

#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class CollectOperation : public GCOperation {

    /* ===== Constructor ===== */

public:
    explicit CollectOperation(const std::vector<GCNode*>& entryNodes) {
        for (GCNode* entryNode : entryNodes)
            queue.push(entryNode);
    }



    /* ===== Operation ===== */

private:
    std::queue<GCNode*> queue;
    std::unordered_set<GCNode*> visited;

public:
    bool step() override {

        GCNode* entryNode = queue.front();
        queue.pop();



        return queue.empty();
    }

};

} // Spark
