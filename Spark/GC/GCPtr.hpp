#pragma once

namespace Spark {

class GCNode;

template <typename T>
class GCPtr {

    /* ===== Friend Class ===== */

    friend class GC;



    /* ===== Data ===== */

private:
    GCNode* nodePtr;

public:
    [[nodiscard]]
    size_t referenceCount() const;



    /* ===== Constructor & Destructor ===== */

private:
    explicit GCPtr(GCNode* nodePtr);

public:
    ~GCPtr();



    /* ===== Copying ===== */

public:
    GCPtr(const GCPtr<T>& other);

    GCPtr<T>& operator=(const GCPtr<T>& other);



    /* ===== Operators ===== */

public:
    T& operator*();

    T* operator->();

};

} // Spark

#include "GCPtr.tpp"
