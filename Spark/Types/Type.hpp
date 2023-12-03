#pragma once
#include <cstdint>

namespace Spark {

enum class Types : uint8_t {
    None,
    Type,
    Integer,
    Float,
    Boolean,
    Pointer,
    String,
    Object,
    Array,
    Table
};

struct Type {

    /* ===== Data ===== */

private:
    Types value;



    /* ===== Constructors ===== */

public:
    constexpr Type() : value(Types::None) { }
    constexpr Type(const Types type) : value(type) { }



    /* ===== Conversion Operators ===== */

public:
    constexpr operator Types() const { return value; }



    /* ===== Operators ===== */

public:
    constexpr Type& operator=(const Types type) {
        value = type;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Type type) {
        switch (type.value) {
            case Types::None:
                os << "None";
                break;

            case Types::Type:
                os << "Type";
                break;

            case Types::Integer:
                os << "Integer";
                break;

            case Types::Float:
                os << "Float";
                break;

            case Types::Boolean:
                os << "Boolean";
                break;

            case Types::Pointer:
                os << "Pointer";
                break;

            case Types::String:
                os << "String";
                break;

            case Types::Object:
                os << "Object";
                break;

            case Types::Array:
                os << "Array";
                break;

            case Types::Table:
                os << "Table";
                break;
        }
        return os;
    }

};

}
