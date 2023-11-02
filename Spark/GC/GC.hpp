#pragma once

#include "../Types/TypeTrait.hpp"

namespace Spark {

    template <typename T>
    class GCHandle;

    class GC {

        /* =====  ===== */

    public:
        template <typename T>
        GCHandle<T> make(T value = {}) {

            // Make sure the type is a Spark type
            static_assert(isSparkReferenceType<T>, "Spark::GC can only make Spark reference types.");


        }

    };

} // Spark
