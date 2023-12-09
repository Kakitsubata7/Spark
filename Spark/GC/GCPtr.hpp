#pragma once

#include "GCNode.hpp"

namespace Spark {

template <typename T>
class GCPtr {

    /* ===== Data ===== */

private:
    struct Data {
    public:
        size_t count;
        GCNode<T>* node;

        explicit Data(size_t count = 0, GCNode<T>* node = nullptr) : count(count), node(node) { }
    };

    Data* data;

public:
    [[nodiscard]]
    constexpr size_t count() const { return data->count; }



    /* ===== Constructor ===== */

private:
    explicit GCPtr(const T& value) : data(new Data(1, new GCNode<T>(value))) { }



    /* ===== Factory Methods ===== */

public:
    template <typename... Args>
    [[nodiscard]]
    static GCPtr<T> make(Args&&... args) {
        return GCPtr<T>(T(std::forward<Args>(args)...));
    }



    /* ===== Copy Assignment Operator ===== */

public:
//    GCPtr& operator=(const GCPtr& other) {
//        if (this != &other) {
//            release();  // Release current resource
//            ptr = other.ptr;
//            refCount = other.refCount;
//            if (refCount) {
//                ++(*refCount);  // Increment ref count of new resource
//            }
//        }
//        return *this;
//    }



    /* ===== Operators ===== */

public:
    T& operator*() {
        return data->node->data();
    }

};

} // Spark
