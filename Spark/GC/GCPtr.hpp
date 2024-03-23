#pragma once

#include <memory>

#include "GCNode.hpp"

namespace Spark {

template <typename T>
class GCPtr {

    /* ===== Data ===== */

private:
    GCNode* _node;

public:
    [[nodiscard]]
    constexpr GCNode* node() const {
        return _node;
    }



    /* ===== Constructor ===== */

public:
    explicit GCPtr(GCNode* node) : _node(node) { }

};

} // Spark
