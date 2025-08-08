#pragma once

#include <cstdint>
#include <unordered_set>
#include <vector>

#include "rc.hpp"

namespace Spark::Runtime {

struct DRCNode;

/**
 * Represents the header of a Spark object that uses double reference counting.
 */
struct DRCHeader {
    DRCNode* node;
    RCHeader rcHeader;
};

struct DRCNode {
    DRCHeader* obj;
    RCInt internalRefCount;
    std::vector<DRCNode*> referencees;
    uintptr_t traversalId;

    bool operator==(const DRCNode& other) const {
        return obj == other.obj;
    }
};

} // Spark::Runtime

namespace std {
    template <>
    struct hash<Spark::Runtime::DRCNode> {
        size_t operator()(const Spark::Runtime::DRCNode& node) const noexcept {
            return std::hash<Spark::Runtime::DRCHeader*>{}(node.obj);
        }
    };
} // std

namespace Spark::Runtime {

class DRC {
private:
    std::unordered_set<DRCNode> _nodes;

    std::vector<DRCNode*> _toRemoveCache;
    std::vector<DRCNode*> _stackCache;

    uintptr_t _traversalId = 0;

public:
    DRC() = default;

    /**
     * Adds a new DRC node to the DRC graph with associated with a given DRC object.
     *
     * @param obj DRC object associated with the node.
     */
    DRCNode* add(DRCHeader* obj);

    /**
     * A DRC object
     *
     */
    void retain(DRCNode* owner, DRCNode* referencee);

    const std::vector<DRCNode*>& release(DRCNode* owner, DRCNode* referencee);

    const std::vector<DRCNode*>& tryCleanup(DRCNode* from);

private:
    uintptr_t getNewTraversalId();
};

} // Spark::Runtime
