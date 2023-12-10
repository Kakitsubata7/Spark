#pragma once

#include "GCNode.hpp"

namespace Spark {

template <typename T>
class GCPtr {

    /* ===== Data ===== */

private:
    T* dataPtr;
    GCNode* nodePtr;

public:
    [[nodiscard]]
    constexpr T* getDataPtr() { return dataPtr; }

    [[nodiscard]]
    constexpr GCNode* getNodePtr() { return nodePtr; }

    [[nodiscard]]
    constexpr size_t referenceCount() { return nodePtr->getRefCount(); }



    /* ===== Constructor ===== */

public:
    explicit GCPtr(const T& value)  {
        dataPtr = new T();
        *dataPtr = value;
        nodePtr = new GCNode(1);
    }



    /* ===== Operators ===== */

public:
    T& operator*() {
        return *dataPtr;
    }

    T* operator->() {
        return dataPtr;
    }

};

} // Spark
