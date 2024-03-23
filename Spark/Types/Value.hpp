#pragma once

#include "Bool.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "Type.hpp"

namespace Spark {

class Value {

    /* ===== Constructor, Factory Methods & Destructor ===== */

public:
    Value() {
        type = Type::Nil;
        intValue = {};
        ptrValue = {};
    }

    static Value makeNil() {
        Value self;
        self.type = Type::Nil;
        return self;
    }

    static Value makeInt(Int value) {
        Value self;
        self.type = Type::Integer;
        self.intValue = value;
        return self;
    }

    static Value makeFloat(Float value) {
        Value self;
        self.type = Type::Float;
        self.floatValue = value;
        return self;
    }

    static Value makeBool(Bool value) {
        Value self;
        self.type = Type::Boolean;
        self.boolValue = value;
        return self;
    }

    static Value makeType(Type value) {
        Value self;
        self.type = Type::Type;
        self.typeValue = value;
        return self;
    }

    ~Value() = default;



    /* ===== Data ===== */

public:
    Type type;

    union {
        Int intValue{};
        Float floatValue;
        Bool boolValue;
        void* ptrValue;
        Type typeValue;
    };

    [[nodiscard]]
    bool isReferenceType() const {
        return static_cast<int>(type) > 0x04;
    }

};

} // Spark
