#pragma once

#include <cstdint>

namespace Spark {

    enum class Type : uint8_t {
        Nil,
        Integer,
        Float,
        Boolean,
        Pointer,
        Type,
        String,
        Array,
        Set,
        Map,
        Object,
        Function,
        Thread,
        Promise
    };

} // Spark
