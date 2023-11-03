#pragma once

#include "../Types/TypeTrait.hpp"
#include "GC.hpp"

namespace Spark {

    template <typename T>
    class GCHandle {

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
