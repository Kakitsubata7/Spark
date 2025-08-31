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

    bool operator==(const DRCNode& other) const noexcept {
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

/**
 * Represents a DRC graph that associates DRC objects with DRC nodes to manage their lifetimes.
 */
class DRC {
private:
    std::unordered_set<DRCNode> _nodes;

    std::vector<DRCNode*> _toRemoveCache;
    std::vector<DRCNode*> _stackCache;

    /**
     * Current traversal ID.
     */
    uintptr_t _traversalId = 0;

public:
    DRC() = default;

    /**
     * Gets the number of nodes in this DRC graph.
     *
     * @return Number of nodes in this DRC graph.
     */
    [[nodiscard]]
    constexpr size_t nodeCount() const noexcept { return _nodes.size(); }

    /**
     * Adds a new DRC node to the DRC graph with associated with a given DRC object.
     *
     * @param obj DRC object associated with the node.
     * @return Pointer to the newly created DRC node.
     */
    DRCNode* add(DRCHeader* obj) noexcept;

    /**
     * A DRC node references another DRC node.
     *
     * @param owner DRC node that references referencee.
     * @param referencee DRC node that gets referenced by owner.
     */
    void retain(DRCNode* owner, DRCNode* referencee) noexcept;

    /**
     * A DRC node releases (one) reference for the other DRC node.
     *
     * @param owner DRC node that references referencee.
     * @param referencee DRC node that is referencing by owner.
     * @return Array of DRC nodes that were deleted.
     */
    const std::vector<DRCNode*>& release(DRCNode* owner, DRCNode* referencee) noexcept;

    /**
     * Tries to start cleaning up from a node.
     *
     * @param from Node where the cleanup starts.
     * @return Array of DRC nodes that were deleted during the cleanup.
     */
    const std::vector<DRCNode*>& tryCleanup(DRCNode* from) noexcept;

private:
    /**
     * Gets a new traversal traversal ID.
     *
     * @return New traversal ID.
     */
    uintptr_t getNewTraversalId() noexcept;
};

} // Spark::Runtime
