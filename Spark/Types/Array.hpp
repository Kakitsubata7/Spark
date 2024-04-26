#pragma once

#include "../Thread.hpp"
#include "Int.hpp"
#include "Value.hpp"

namespace Spark {

namespace Array {

    /**
     * Parameters:
     *     [1] Array reference
     *     [2] Value to append
     */
    inline Int append(Thread* th) {
        // TODO: Provide actual implementation
        Value& array = th->getArg(1);
        Value& item = th->getArg(2);
        array.node->data<std::vector<Value>>().push_back(item);
        return 0;
    }

} // Array

} // Spark
