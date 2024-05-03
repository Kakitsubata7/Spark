#pragma once

#include <string>

#include "Bool.hpp"
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
        Bool boolValue;
        std::string strValue;
    };



    /* ===== Constructors, Factory Methods & Destructor ===== */

public:
    Value() = default;

    Value(const Value& other);
    Value(Value&& other) noexcept;

    ~Value();

    [[nodiscard]]
    static Value makeNil();

    [[nodiscard]]
    static Value makeInt(Int i);

    [[nodiscard]]
    static Value makeFloat(Float f);

    [[nodiscard]]
    static Value makeBool(Bool b);

    [[nodiscard]]
    static Value makeString(const std::string& str);



    /* ===== Operators ===== */

public:
    Value& operator=(const Value& other);
    Value& operator=(Value&& other) noexcept;

    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    Value operator%(const Value& other) const;

};

std::ostream& operator<<(std::ostream& os, const Value& value);

} // Spark
