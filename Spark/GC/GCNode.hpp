#pragma once

#include <forward_list>

namespace Spark {

class GCNode {

    /* ===== Data ===== */

private:
    long referenceCount = 0;
    std::forward_list<GCNode*> neighbors;

public:
    [[nodiscard]]
    constexpr const std::forward_list<GCNode*>& getNeighbors() const {
        return neighbors;
    }

    bool isMarked = false;



    /* ===== Constructor ===== */

public:
    GCNode() = default;

};

} // Spark
