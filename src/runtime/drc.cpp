#include "drc.hpp"

#include "thread.hpp"

namespace Spark::Runtime {

/**
 * Swap removes referencee from owner's referencee list.
 *
 * @param owner Owner node (to remove referencee from).
 * @param referencee Node that's being referenced by owner (to be removed from owner).
 */
static bool removeReferencee(DRCNode* owner, DRCNode* referencee) noexcept {
    std::vector<DRCNode*>& referencees = owner->referencees;
    DRCNode* referenceeNode = referencee;
    for (size_t i = 0; referencees.size(); ++i) {
        if (referencees[i] == referenceeNode) {
            referencees[i] = referencees.back();
            referencees.pop_back();
            return true;
        }
    }
    return false;
}

DRCNode* DRC::add(DRCHeader* obj) noexcept {
    auto it = _nodes.emplace().first;
    DRCNode* node = const_cast<DRCNode*>(&*it);
    node->obj = obj;
    return node;
}

void DRC::retain(DRCNode* owner, DRCNode* referencee) noexcept {
    owner->referencees.push_back(referencee);
    referencee->internalRefCount++;
}

const std::vector<DRCNode*>& DRC::release(DRCNode* owner, DRCNode* referencee) noexcept {
    // Remove referencee node from owner's referencees
    if (!removeReferencee(owner, referencee)) {
        _toRemoveCache.clear();
        return _toRemoveCache;
    }
    referencee->internalRefCount--;

    return tryCleanup(referencee);
}

const std::vector<DRCNode*>& DRC::tryCleanup(DRCNode* from) noexcept {
    // Ignore if the starting node is still externally referenced
    if (from->obj->rcHeader.refCount > 0) {
        _toRemoveCache.clear();
        return _toRemoveCache;
    }

    // Trial removal
    uintptr_t traversalId = getNewTraversalId();
    _toRemoveCache.clear();
    std::vector<DRCNode*>& toRemove = _toRemoveCache;
    toRemove.push_back(from);
    _stackCache.clear();
    std::vector<DRCNode*>& stack = _stackCache;
    stack.push_back(from);
    while (!stack.empty()) {
        DRCNode* node = stack.back();
        stack.pop_back();
        if (node->traversalId == traversalId) {
            continue;
        }
        node->traversalId = traversalId; // Update traversal ID to mark it as visited

        for (DRCNode* referencee : node->referencees) {
            // Decrease internal RC
            referencee->internalRefCount--;

            // Stop if it's externally referenced
            if (referencee->obj->rcHeader.refCount > 0) {
                continue;
            }

            // Mark as to remove if internal RC reaches zero (the source node is always marked as to remove, so ignore it)
            if (referencee->internalRefCount == 0 && referencee != from) {
                toRemove.push_back(referencee);
            }

            // Push to the stack
            stack.push_back(referencee);
        }
    }

    // Check if any node is still being referenced
    bool abort = false;
    for (DRCNode* node : toRemove) {
        if (node->internalRefCount > 0 || node->obj->rcHeader.refCount > 0) {
            abort = true;
            break;
        }
    }
    if (abort) {
        // Resume internal RCs
        traversalId = getNewTraversalId();
        stack.clear();
        stack.push_back(from);
        while (!stack.empty()) {
            DRCNode* node = stack.back();
            stack.pop_back();
            node->traversalId = traversalId; // Update traversal ID to mark it as visited

            for (DRCNode* referencee : node->referencees) {
                referencee->internalRefCount++;
                if (referencee->traversalId != traversalId) {
                    stack.push_back(referencee);
                }
            }
        }

        toRemove.clear();
    }

    return toRemove;
}

uintptr_t DRC::getNewTraversalId() noexcept {
    _traversalId++;
    if (_traversalId == 0) {
        for (const DRCNode& node : _nodes) {
            const_cast<DRCNode&>(node).traversalId = 0;
        }
        _traversalId = 1;
    }

    return _traversalId;
}

} // Spark::Runtime
