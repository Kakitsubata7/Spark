#pragma once

#include <list>

namespace Spark {

class GCNode {

    /* ===== Data ===== */

private:
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
    GCNode(void* dataPtr, void (*destructorPtr)(void*)) : dataPtr(dataPtr),
                                                          destructorPtr(destructorPtr) { }

    ~GCNode() {
        destructorPtr(dataPtr);
        ::operator delete(dataPtr);
    }

};

} // Spark
