#pragma once

#include <unordered_set>

namespace Spark {

class GCNode {

    /* ===== Data ===== */

private:
    size_t refCount = 0;
    std::unordered_set<GCNode*> referencedNodeSet;

public:
    [[nodiscard]]
    constexpr size_t getRefCount() const { return refCount; }



    /* ===== Constructor & Destructor ===== */

public:
    explicit GCNode(size_t initialRefCount) : refCount(initialRefCount) { }

    ~GCNode() {
        for (GCNode* node : referencedNodeSet)
            dereference(node);
    }



    /* ===== Reference & Deference ===== */

public:
    void reference(GCNode* target) {
        target->refCount++;
        referencedNodeSet.insert(target);
    }

    void dereference(GCNode* target) {
        target->refCount--;
        referencedNodeSet.erase(target);
    }

};

} // Spark
