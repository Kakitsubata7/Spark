#pragma once

#include <string>

#include "Int.hpp"
#include "Float.hpp"
#include "Type.hpp"

namespace Spark {

struct Value {

    /* ===== Data ===== */

public:
    Type type = Type::Nil;

    union {
        Int intValue{};
        Float floatValue;
        std::string strValue;
    };



    /* ===== Constructors & Destructor ===== */

public:
    Value() = default;
    Value(const Value& other);
    Value(Value&& other) noexcept;

    ~Value();



    /* ===== Operators ===== */

public:
    Value operator+(const Value& other);
    Value operator-(const Value& other);
    Value operator*(const Value& other);
    Value operator/(const Value& other);
    Value operator%(const Value& other);

};

} // Spark
