#pragma once

namespace Spark {

    /* ===== Constructors ===== */

    template <typename T>
    GCHandle<T>::GCHandle(const T& value, GC& gc) : gc(gc) {
        // TODO: Implement GCHandle<T>::GCHandle(const T& value, GC& gc)
    }

    template <typename T>
    GCHandle<T>::GCHandle(GC &gc) : GCHandle(T{}, gc) { }



    /* ===== Destructor ===== */

    template <typename T>
    GCHandle<T>::~GCHandle() {



    }

} // Spark