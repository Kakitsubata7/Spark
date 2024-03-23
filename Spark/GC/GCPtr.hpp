#pragma once

#include "GCNode.hpp"

namespace Spark {

template <typename T>
class GCPtr {

    /* ===== Data ===== */

private:
    GCNode* node;

public:
    [[nodiscard]]
    constexpr GCNode* getGCNode() const {
        return node;
    }



    /* ===== Constructor ===== */

};

} // Spark
