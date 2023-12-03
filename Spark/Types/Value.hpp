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
    Type _type;
    Boolean _isConstant;
    union {
        Type typeValue;
        Integer integerValue;
        Float floatValue;
        Boolean booleanValue;
        Pointer pointerValue;
    };

public:
    [[nodiscard]]
    constexpr Type type() const { return _type; }

    [[nodiscard]]
    constexpr Boolean isConstant() const { return _isConstant; }



    /* ===== Constructors ===== */

private:
    Value() : _type(Types::None), _isConstant(false), pointerValue({}) { }

public:
    Value(const Value& value) : _type(value._type), _isConstant(value._isConstant) {
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



    /* ===== Conversion Operators ===== */



    /* ===== Operators ===== */

public:
    Value& operator=(const Value& value) {
        _type = value._type;
        _isConstant = value._isConstant;
        switch (value._type) {
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

    Value& operator=(const Type type) {
        if (_isConstant)
            throw std::runtime_error("Value is constant so it cannot be reassigned.");

        this->_type = Types::Type;
        typeValue = type;
        return *this;
    }

    Value& operator=(const Integer integer) {
        if (_isConstant)
            throw std::runtime_error("Value is constant so it cannot be reassigned.");

        _type = Types::Integer;
        integerValue = integer;
        return *this;
    }

    Value& operator=(const Float f) {
        if (_isConstant)
            throw std::runtime_error("Value is constant so it cannot be reassigned.");

        _type = Types::Float;
        floatValue = f;
        return *this;
    }

    Value& operator=(const Boolean boolean) {
        if (_isConstant)
            throw std::runtime_error("Value is constant so it cannot be reassigned.");

        _type = Types::Boolean;
        booleanValue = boolean;
        return *this;
    }

    Value& operator=(const Pointer pointer) {
        if (_isConstant)
            throw std::runtime_error("Value is constant so it cannot be reassigned.");

        _type = Types::Pointer;
        pointerValue = pointer;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Value& value) {
        switch (value._type) {
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
