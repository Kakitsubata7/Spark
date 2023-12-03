#include "Value.hpp"

namespace Spark {

    /* ===== Factory Methods ===== */

    template<>
    Value Value::make<Type>(const Type value) {
        Value result;
        result.type = Types::Type;
        result.isConst = false;
        result.typeValue = value;
        return result;
    }

    template<>
    Value Value::make<Integer>(const Integer value) {
        Value result;
        result.type = Types::Integer;
        result.isConst = false;
        result.integerValue = value;
        return result;
    }

    template<>
    Value Value::make<Float>(const Float value) {
        Value result;
        result.type = Types::Float;
        result.isConst = false;
        result.floatValue = value;
        return result;
    }

    template<>
    Value Value::make<Boolean>(const Boolean value) {
        Value result;
        result.type = Types::Boolean;
        result.isConst = false;
        result.booleanValue = value;
        return result;
    }

    template<>
    Value Value::make<Pointer>(const Pointer value) {
        Value result;
        result.type = Types::Pointer;
        result.isConst = false;
        result.pointerValue = value;
        return result;
    }

    template<>
    Value Value::make<Type>() {
        Value result;
        result.type = Types::Type;
        result.isConst = false;
        result.typeValue = Types::None;
        return result;
    }

    template<>
    Value Value::make<Integer>() {
        Value result;
        result.type = Types::Integer;
        result.isConst = false;
        result.integerValue = 0;
        return result;
    }

    template<>
    Value Value::make<Float>() {
        Value result;
        result.type = Types::Float;
        result.isConst = false;
        result.floatValue = 0.0;
        return result;
    }

    template<>
    Value Value::make<Boolean>() {
        Value result;
        result.type = Types::Boolean;
        result.isConst = false;
        result.booleanValue = false;
        return result;
    }

    template<>
    Value Value::make<Pointer>() {
        Value result;
        result.type = Types::Pointer;
        result.isConst = false;
        result.pointerValue = nullptr;
        return result;
    }

    template<>
    const Value Value::makeConstant<Type>(const Type value) {
        Value result;
        result.type = Types::Type;
        result.isConst = true;
        result.typeValue = value;
        return result;
    }

    template<>
    const Value Value::makeConstant<Integer>(const Integer value) {
        Value result;
        result.type = Types::Integer;
        result.isConst = true;
        result.integerValue = value;
        return result;
    }

    template<>
    const Value Value::makeConstant<Float>(const Float value) {
        Value result;
        result.type = Types::Float;
        result.isConst = true;
        result.floatValue = value;
        return result;
    }

    template<>
    const Value Value::makeConstant<Boolean>(const Boolean value) {
        Value result;
        result.type = Types::Boolean;
        result.isConst = true;
        result.booleanValue = value;
        return result;
    }

    template<>
    const Value Value::makeConstant<Pointer>(const Pointer value) {
        Value result;
        result.type = Types::Pointer;
        result.isConst = true;
        result.pointerValue = value;
        return result;
    }

    template<>
    const Value Value::makeConstant<Type>() {
        Value result;
        result.type = Types::Type;
        result.isConst = true;
        result.typeValue = Types::None;
        return result;
    }

    template<>
    const Value Value::makeConstant<Integer>() {
        Value result;
        result.type = Types::Integer;
        result.isConst = true;
        result.integerValue = 0;
        return result;
    }

    template<>
    const Value Value::makeConstant<Float>() {
        Value result;
        result.type = Types::Float;
        result.isConst = true;
        result.floatValue = 0.0;
        return result;
    }

    template<>
    const Value Value::makeConstant<Boolean>() {
        Value result;
        result.type = Types::Boolean;
        result.isConst = true;
        result.booleanValue = false;
        return result;
    }

    template<>
    const Value Value::makeConstant<Pointer>() {
        Value result;
        result.type = Types::Pointer;
        result.isConst = true;
        result.pointerValue = nullptr;
        return result;
    }

}

