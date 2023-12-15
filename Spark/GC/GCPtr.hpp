#pragma once

#include "GCNode.hpp"

namespace Spark {

template <typename T>
class GCPtr {

    /* ===== Friend Class ===== */

    friend class GC;



    /* ===== Data ===== */

private:
    GCNode* nodePtr;

public:
    [[nodiscard]]
    size_t referenceCount() const {
        return nodePtr->referenceCount;
    }



    /* ===== Constructor & Destructor ===== */

private:
    explicit GCPtr(GCNode* nodePtr) : nodePtr(nodePtr) { }

public:
    ~GCPtr() {
        nodePtr->referenceCount--;
        if (nodePtr->referenceCount == 0)
            nodePtr->collect();
    }



    /* ===== Copying ===== */

public:
    GCPtr(const GCPtr<T>& other) : nodePtr(other.nodePtr) {
        nodePtr->referenceCount++;
    }

    GCPtr<T>& operator=(const GCPtr<T>& other) {
        if (this != &other) {
            nodePtr = other.nodePtr;
            nodePtr->referenceCount++;
        }
        return *this;
    }



    /* ===== Operators ===== */

public:
    T& operator*() {
        return *(reinterpret_cast<T*>(nodePtr->dataPtr));
    }

    T* operator->() {
        return reinterpret_cast<T*>(nodePtr->dataPtr);
    }

};

} // Spark
