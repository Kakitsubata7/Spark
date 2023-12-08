#pragma once

#include "GCNode.hpp"

namespace Spark {

template <typename T>
class GCPtr {

    /* ===== Data ===== */

private:
    class {
        size_t count = 0;
        GCNode<T>* node = nullptr;
    }* data;



//    /* ===== Constructor ===== */
//
//private:
//
//
//
//    /* ===== Factory Methods ===== */
//
//public:
//    template <typename... Args>
//    static GCPtr<T> make(Args&&... args) {
//        return GCPtr<T>(new T(std::forward<Args>(args)...));
//    }

};

} // Spark
