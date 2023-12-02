#pragma once

#include "Boolean.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "Type.hpp"

namespace Spark {

class Value {

    /* ===== Data ===== */

private:
    Type type;
    Boolean isConstant;
    union {
        Type typeValue;
        Integer integerValue;
        Float floatValue;
        Boolean booleanValue;
        void* pointerValue{};
    };



    /* ===== Constructors ===== */

public:
    Value() : type(Types::None), isConstant(false) {
        integerValue = 0;
        pointerValue = nullptr;
    }

};

} // Spark
