#pragma once

#include "Bool.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "Type.hpp"

namespace Spark {

class Value {

public:
    Type type;

    union {
        Int intValue;
        Float floatValue;
        Bool boolValue;
        void* ptrValue;
        Type typeValue;
    };

public:
    Value() {
        type = Type::Nil;
        ptrValue = {};
        intValue = {};
    }

};

} // Spark
