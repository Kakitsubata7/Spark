#pragma once
#include <cstdint>

namespace Spark {

struct Type {

    /* ===== Data ===== */

public:
    enum class TypeEnum : uint8_t {
        None,
        Type,
        Integer,
        Float,
        Boolean,
        String,
        Object,
        Array,
        Table
    };

private:
    constexpr TypeEnum value;



    /* ===== Constructors ===== */

public:
    constexpr Type() : value(TypeEnum::None) { }
    constexpr Type(const TypeEnum type) : value(type) { }



    /* ===== Conversion Operators ===== */

public:
    constexpr operator TypeEnum() const { return value; }



    /* ===== Constants ===== */

public:
    static constexpr Type None = TypeEnum::None;
    static constexpr Type Integer = TypeEnum::Integer;
    static constexpr Type Float = TypeEnum::Float;
    static constexpr Type Boolean = TypeEnum::Boolean;
    static constexpr Type String = TypeEnum::String;
    static constexpr Type Object = TypeEnum::Object;
    static constexpr Type Array = TypeEnum::Array;
    static constexpr Type Table = TypeEnum::Table;

};

}
