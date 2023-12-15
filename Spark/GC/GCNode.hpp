#pragma once

#include <unordered_set>
#include <iostream>

namespace Spark {

class GC;

class GCNode {

    /* ===== Friend Classes ===== */

    friend class GC;
    template <typename> friend class GCPtr;



    /* ===== Data ===== */

private:
    size_t referenceCount;
    GC* gcPtr;
    void* dataPtr;
    void (*destructorPtr)(void*);
    std::unordered_set<GCNode*> nodeReferencingSet;



    /* ===== Constructor & Destructor ===== */

private:
    explicit GCNode(size_t initialRefCount,
                    GC* gcPtr,
                    void* dataPtr,
                    void (*destructorPtr)(void*)) : referenceCount(initialRefCount),
                                                    gcPtr(gcPtr),
                                                    dataPtr(dataPtr),
                                                    destructorPtr(destructorPtr) { }

    ~GCNode() {
        destructorPtr(dataPtr);
        ::operator delete(dataPtr);
    }



    /* ===== Operations ===== */

private:
    void collect();

    void reference(GCNode* target) {
        nodeReferencingSet.insert(target);
        target->referenceCount++;
    }

    void dereference(GCNode* target) {
        nodeReferencingSet.erase(target);
        target->referenceCount--;
    }

};

} // Spark
