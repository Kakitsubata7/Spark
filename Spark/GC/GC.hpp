#pragma once

#include <unordered_set>

#include "GCPtr.hpp"
#include "GCNode.hpp"

namespace Spark {

class GC {

    /* ===== Data ===== */

private:
    std::unordered_set<GCNode*> trackingNodeSet;

public:
    constexpr const std::unordered_set<GCNode*>& getTrackingNodeSet() const {
        return trackingNodeSet;
    }



    /* ===== Constructors ===== */

public:
    GC() = default;



    /* ===== Factory Methods ===== */

public:
    template <typename T, typename... Args>
    [[nodiscard]]
    GCPtr<T> make(Args&&... args) {
        GCPtr<T> ptr(T(std::forward<Args>(args)...));
        trackingNodeSet.insert(ptr.getNodePtr());
        return ptr;
    }

};

} // Spark
