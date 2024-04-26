#pragma once

#include <list>

namespace Spark {

class GC;

class GCNode {

    /* ===== Data ===== */

private:
    void* dataPtr;
    void (*destructorPtr)(void*);
    GC* gcPtr;

    std::list<GCNode*> _neighbors;

public:
    [[nodiscard]]
    constexpr GC& getGC() const {
        return *gcPtr;
    }

    [[nodiscard]]
    constexpr const std::list<GCNode*>& neighbors() const {
        return _neighbors;
    }

    [[nodiscard]]
    constexpr std::list<GCNode*>& neighbors() {
        return _neighbors;
    }

    bool isMarked = false; // Used for GC collection
    long referenceCount = 0;

    template <typename T>
    [[nodiscard]]
    T& data() {
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
    GCNode(void* dataPtr, void (*destructorPtr)(void*), GC* gcPtr) : dataPtr(dataPtr),
                                                                     destructorPtr(destructorPtr),
                                                                     gcPtr(gcPtr) { }

public:
    template <typename T>
    static GCNode make(const T& value, GC& gc) {
        void* dataPtr = new T(value);
        void (*destructorPtr)(void*) = [](void* obj) { static_cast<T*>(obj)->~T(); };
        return GCNode(dataPtr, destructorPtr, &gc);
    }

    ~GCNode() {
        destructorPtr(dataPtr);
        ::operator delete(dataPtr);
    }

};

} // Spark
