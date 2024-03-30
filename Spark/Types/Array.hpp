#pragma once

#include "../Thread.hpp"
#include "Int.hpp"
#include "Value.hpp"

namespace Spark {

namespace Array {

    /**
     * Parameters:
     *     [0] Array reference
     *     [1] Value to append
     */
    inline Int append(Thread* th) {
        // TODO: Provide actual implementation
        Value& array = th->getArg(0);
        Value& item = th->getArg(1);
        array.nodePtr->getData<std::vector<Value>>().push_back(item);
        return 0;
    }

} // Array

} // Spark
