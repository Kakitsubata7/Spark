#pragma once

#include <list>

namespace Spark {

class GC;

class GCNode {

    /* ===== Data ===== */

private:
    GC* gcPtr;
    void* dataPtr;
    void (*destructorPtr)(void*);

    long _referenceCount = 0;
    std::list<GCNode*> _neighbors;

public:
    [[nodiscard]]
    constexpr long referenceCount() const {
        return _referenceCount;
    }

    [[nodiscard]]
    constexpr const std::list<GCNode*>& neighbors() const {
        return _neighbors;
    }

    bool isMarked = false;



    /* ===== Constructor & Destructor ===== */

public:
    GCNode(GC* gcPtr, void* dataPtr, void (*destructorPtr)(void*)) : gcPtr(gcPtr),
                                                                     dataPtr(dataPtr),
                                                                     destructorPtr(destructorPtr) { }

    ~GCNode() {
        destructorPtr(dataPtr);
        ::operator delete(dataPtr);
    }

};

} // Spark
