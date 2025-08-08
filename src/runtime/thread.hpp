#include <cstddef>
#include <unordered_set>

#include "drc.hpp"

namespace Spark::Runtime {

/**
 * Represents a thread in a Spark context.
 */
template <typename Allocator>
class Thread {
private:
    Allocator _allocator;

    /**
     * Set of every allocated Spark RC objects of this thread.
     */
    std::unordered_set<RCHeader*> _rcObjects;

    /**
     * Set of every allocated Spark DRC objects of this thread.
     */
    std::unordered_set<DRCHeader*> _drcObjects;

    /**
     * Double reference object graph.
     */
    DRC _drc;

public:
    explicit Thread(Allocator allocator = {}) noexcept : _allocator(std::move(allocator)) { }

    ~Thread() noexcept {
        for (RCHeader* obj : _rcObjects) {
            obj->type->destruct(obj);
            _allocator.free(obj);
        }
        for (DRCHeader* obj : _drcObjects) {
            obj->rcHeader.type->destruct(obj);
            _allocator.free(obj);
        }
    }

    static RCHeader* newRCObject(Thread* th, const Type* type) noexcept {
        RCHeader* obj = reinterpret_cast<RCHeader*>(th->_allocator.alloc(type->size()));
        obj->refCount = 0;
        obj->type = type;
        th->_rcObjects.insert(obj);
        return obj;
    }

    static void deleteRCObject(Thread* th, RCHeader* obj) noexcept {
        obj->type->destruct(obj);
        th->_allocator.free(obj);
    }

    static DRCHeader* newDRCObject(Thread* th, const Type* type) noexcept {
        DRCHeader* obj = static_cast<DRCHeader*>(th->_allocator.alloc(type->size()));
        obj->rcHeader.refCount = 0;
        obj->rcHeader.type = type;
        obj->node = th->_drc.add(obj);
        th->_drcObjects.insert(obj);
        return obj;
    }

    static void drcRetainDRC(Thread* th, DRCHeader* owner, DRCHeader* referencee) noexcept {
        th->_drc.retain(owner->node, referencee->node);
    }

    static void drcReleaseDRC(Thread* th, DRCHeader* owner, DRCHeader* referencee) noexcept {
        const std::vector<DRCNode*>& toDelete = th->_drc.release(owner->node, referencee->node);
        for (DRCNode* node : toDelete) {
            DRCHeader* obj = node->obj;
            node->obj->rcHeader.type->destruct(obj);
            th->_allocator.free(obj);
        }
    }
};

} // Spark::Runtime
