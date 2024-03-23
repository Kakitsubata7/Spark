#pragma once

#include <list>

namespace Spark {

class GCNode {

    /* ===== Data ===== */

private:
    void* dataPtr;
    void (*destructorPtr)(void*);

    std::list<GCNode*> _neighbors;

public:
    [[nodiscard]]
    constexpr const std::list<GCNode*>& neighbors() const {
        return _neighbors;
    }

    [[nodiscard]]
    constexpr std::list<GCNode*>& neighbors() {
        return _neighbors;
    }

    long referenceCount = 0;
    bool isMarked = false;



    /* ===== Constructor & Destructor ===== */

public:
    GCNode(void* dataPtr, void (*destructorPtr)(void*)) : dataPtr(dataPtr),
                                                          destructorPtr(destructorPtr) { }

    ~GCNode() {
        destructorPtr(dataPtr);
        ::operator delete(dataPtr);
    }



    /* ===== Operations ===== */

public:
    void reference(GCNode* other) { }
    void unreference(GCNode* other) { }

};

} // Spark
