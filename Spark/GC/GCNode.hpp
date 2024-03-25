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

    template <typename T>
    [[nodiscard]]
    T& getData() {
        T* p = static_cast<T*>(dataPtr);
        return *p;
    }

    template <typename T>
    [[nodiscard]]
    const T& getData() const {
        T* p = static_cast<T*>(dataPtr);
        return *p;
    }



    /* ===== Constructor, Factory Method & Destructor ===== */

private:
    GCNode(void* dataPtr, void (*destructorPtr)(void*)) : dataPtr(dataPtr),
                                                          destructorPtr(destructorPtr) { }

public:
    template <typename T>
    static GCNode make(const T& value) {
        void* dataPtr = new T(value);
        void (*destructorPtr)(void*) = [](void* obj) { static_cast<T*>(obj)->~T(); };
        return GCNode(dataPtr, destructorPtr);
    }

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
