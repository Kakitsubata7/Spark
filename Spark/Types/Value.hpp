#pragma once

#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../GC/GCNode.hpp"
#include "Bool.hpp"
#include "Float.hpp"
#include "Function.hpp"
#include "Int.hpp"
#include "Type.hpp"

namespace Spark {

class Closure;
class GC;
class Thread;

/**
 * The size of this type should be (8 bytes + pointer size + alignment); at least 16 bytes.
 */
class Value {

    /* ===== Constructor, Factory Methods & Destructor ===== */

public:
    Value() {
        type = Type::Nil;
        intValue = {};
        ptrValue = {};
    }

    [[nodiscard]]
    static Value makeNil() {
        Value self;
        self.type = Type::Nil;
        return self;
    }

    [[nodiscard]]
    static Value makeInt(Int value = 0) {
        Value self;
        self.type = Type::Integer;
        self.intValue = value;
        return self;
    }

    [[nodiscard]]
    static Value makeFloat(Float value = 0.0) {
        Value self;
        self.type = Type::Float;
        self.floatValue = value;
        return self;
    }

    [[nodiscard]]
    static Value makeBool(Bool value = false) {
        Value self;
        self.type = Type::Boolean;
        self.boolValue = value;
        return self;
    }

    [[nodiscard]]
    static Value makeFunction(Function funcPtr) {
        Value self;
        self.type = Type::Function;
        self.funcPtr = funcPtr;
        return self;
    }

    [[nodiscard]]
    static Value makeType(Type value) {
        Value self;
        self.type = Type::Type;
        self.typeValue = value;
        return self;
    }

    /**
     * Note: the returned value is not registered as an entry node in the GC.
     */
    [[nodiscard]]
    static Value makeString(GC& gc, const std::string& value = "");

    /**
     * Note: the returned value is not registered as an entry node in the GC.
     */
    [[nodiscard]]
    static Value makeArray(GC& gc, const std::vector<Value>& value = {});

    [[nodiscard]]
    static Value makeClosure(GC& gc, const Closure& value);

    ~Value() = default;



    /* ===== Data ===== */

public:
    Type type;

    union {
        Int intValue{};
        Float floatValue;
        Bool boolValue;
        void* ptrValue;
        Function funcPtr;
        Type typeValue;
        GCNode* nodePtr;
    };

    [[nodiscard]]
    bool isReferenceType() const {
        return static_cast<int>(type) > static_cast<int>(Type::Type);
    }



    /* ===== Operators ===== */

public:
    friend std::ostream& operator<<(std::ostream& os, const Value& value);

    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    Value operator%(const Value& other) const;

    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;

};

} // Spark
