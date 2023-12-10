#include "GCNode.hpp"

namespace Spark {

    /* ===== Data ===== */

    template <typename T>
    size_t GCPtr<T>::referenceCount() const {
        return nodePtr->referenceCount;
    }



    /* ===== Constructor & Destructor ===== */

    template <typename T>
    GCPtr<T>::GCPtr(GCNode* nodePtr) : nodePtr(nodePtr) { }

    template <typename T>
    GCPtr<T>::~GCPtr() {
        nodePtr->referenceCount--;
        if (nodePtr->referenceCount == 0)
            nodePtr->collect();
    }



    /* ===== Copying ===== */

    template <typename T>
    GCPtr<T>::GCPtr(const GCPtr<T>& other) : nodePtr(other.nodePtr) {
        nodePtr->referenceCount++;
    }

    template <typename T>
    GCPtr<T>& GCPtr<T>::operator=(const GCPtr<T>& other) {
        if (this != &other) {
            nodePtr = other.nodePtr;
            nodePtr->referenceCount++;
        }
        return *this;
    }



    /* ===== Operators ===== */

    template <typename T>
    T& GCPtr<T>::operator*() {
        return *(reinterpret_cast<T*>(nodePtr->dataPtr));
    }

    template <typename T>
    T* GCPtr<T>::operator->() {
        return reinterpret_cast<T*>(nodePtr->dataPtr);
    }

} // Spark