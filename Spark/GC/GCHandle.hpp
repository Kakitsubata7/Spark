#pragma once

#include "../Types/TypeTrait.hpp"
#include "GC.hpp"

namespace Spark {

    template <typename T>
    class GCHandle {
        static_assert(isSparkReferenceType<T>, "Spark::GCHandle can only be created with Spark reference types.");

        /* ===== Constructors ===== */

    public:
        GCHandle(const T& value, GC& gc);
        explicit GCHandle(GC& gc);



        /* ===== Destructor ===== */

    public:
        ~GCHandle();



        /* ===== Copying ===== */

    public:
        GCHandle() {

        }



        /* ===== Metadata ===== */

    private:
        GC& gc;

    };

} // Spark

#include "GCHandle.tpp"
