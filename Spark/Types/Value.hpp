#pragma once

#include "Boolean.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "Pointer.hpp"
#include "Type.hpp"
#include "TypeTrait.hpp"

namespace Spark {

class Value {

    /* ===== Data ===== */

private:
    Type type;
    Boolean isConst;
    union {
        Type typeValue;
        Integer integerValue;
        Float floatValue;
        Boolean booleanValue;
        Pointer pointerValue;
    };

public:
    [[nodiscard]]
    Boolean isConstant() const { return isConst; }



    /* ===== Constructors ===== */

private:
    Value() : type(Types::None), isConst(false) {
        integerValue = 0;
        pointerValue = nullptr;
    }

public:
    Value(const Value& value) {
        *this = value;
    }



    /* ===== Factory Methods ===== */

public:
    template <typename T>
    static Value make(T value);

    template <typename T>
    static Value make();

    template <typename T>
    static const Value makeConstant(T value);

    template <typename T>
    static const Value makeConstant();



    /* ===== Operators ===== */

public:
    constexpr Value& operator=(const Value& value) {
        type = value.type;
        isConst = value.isConst;
        switch (value.type) {
            case Types::Type:
                typeValue = value.typeValue;
                break;

            case Types::Integer:
                integerValue = value.integerValue;
                break;

            case Types::Float:
                floatValue = value.floatValue;
                break;

            case Types::Boolean:
                booleanValue = value.booleanValue;
                break;

            case Types::Pointer:
                pointerValue = value.pointerValue;
                break;

                // TODO: Implement String, Object, etc.

            default:
                break;
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Value& value) {
        switch (value.type) {
            case Types::None:
                // TODO: Implement
                break;

            case Types::Type:
                os << value.typeValue;
                break;

            case Types::Integer:
                os << value.integerValue;
                break;

            case Types::Float:
                os << value.floatValue;
                break;

            case Types::Boolean:
                os << value.booleanValue;
                break;

            case Types::Pointer:
                os << value.pointerValue;
                break;

                // TODO: Implement others
        }
        return os;
    }

};

} // Spark
