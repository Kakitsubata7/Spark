#pragma once

#include "Bool.hpp"
#include "Float.hpp"
#include "Integer.hpp"

namespace Spark {

class Value {

    union {
        Int intValue;
        Float floatValue;
        Bool boolValue;
    };
    
};

} // Spark
