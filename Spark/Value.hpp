#pragma once

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Bool8.hpp"
#include "Float64.hpp"
#include "GC/GCPtr.hpp"
#include "Int64.hpp"
#include "Type.hpp"

namespace Spark {

class Value {

    /* ===== Data ===== */

private:
    Type _type;
    Bool8 _isConstant;

    union {
        Int64 integerValue;
        Float64 floatValue;
        Bool8 booleanValue;
        void* pointerValue;
        Type typeValue;
        GCPtr<std::string> stringPtr;
        GCPtr<std::vector<Value>> arrayPtr;
        GCPtr<std::unordered_set<Value>> setPtr;
        GCPtr<std::unordered_map<Value, Value>> mapPtr;
        GCPtr<std::unordered_map<std::string, Value>> objectPtr;
        GCPtr<void*> threadPtr;
        GCPtr<void*> promisePtr;
    };

public:
    [[nodiscard]]
    constexpr Type type() const { return _type; }

    [[nodiscard]]
    constexpr Bool8 isConstant() const { return _isConstant; }



    /* ===== Constructor ===== */

public:
    constexpr Value() : _type(Type::Nil), _isConstant(false), pointerValue(nullptr) { }



    /* ===== Factory Methods ===== */

public:
    template <typename T>
    [[nodiscard]]
    static typename std::enable_if_t<std::is_same_v<short, T> ||
                            std::is_same_v<unsigned short, T> ||
                            std::is_same_v<int, T> ||
                            std::is_same_v<unsigned int, T> ||
                            std::is_same_v<long, T> ||
                            std::is_same_v<unsigned long, T> ||
                            std::is_same_v<long long, T> ||
                            std::is_same_v<unsigned long long, T>, Value>
    make() {
        Value result;
        result._type = Type::Integer;
        result._isConstant = false;
        result.integerValue = 0;
        return result;
    }

    template <typename T>
    [[nodiscard]]
    static typename std::enable_if_t<std::is_same_v<short, T> ||
                                     std::is_same_v<unsigned short, T> ||
                                     std::is_same_v<int, T> ||
                                     std::is_same_v<unsigned int, T> ||
                                     std::is_same_v<long, T> ||
                                     std::is_same_v<unsigned long, T> ||
                                     std::is_same_v<long long, T> ||
                                     std::is_same_v<unsigned long long, T>, Value>
    make(T value) {
        Value result;
        result._type = Type::Integer;
        result._isConstant = false;
        result.integerValue = static_cast<Int64>(value);
        return result;
    }

    template <typename T>
    [[nodiscard]]
    static const typename std::enable_if_t<std::is_same_v<short, T> ||
                                           std::is_same_v<unsigned short, T> ||
                                           std::is_same_v<int, T> ||
                                           std::is_same_v<unsigned int, T> ||
                                           std::is_same_v<long, T> ||
                                           std::is_same_v<unsigned long, T> ||
                                           std::is_same_v<long long, T> ||
                                           std::is_same_v<unsigned long long, T>, Value>
    makeConstant(T value) {
        Value result;
        result._type = Type::Integer;
        result._isConstant = true;
        result.integerValue = static_cast<Int64>(value);
        return result;
    }

    template <typename T>
    [[nodiscard]]
    static typename std::enable_if_t<std::is_same_v<float, T> ||
                            std::is_same_v<double, T> ||
                            std::is_same_v<long double, T>, Value>
    make() {
        Value result;
        result._type = Type::Float;
        result._isConstant = false;
        result.floatValue = 0.0;
        return result;
    }

    template <typename T>
    [[nodiscard]]
    static typename std::enable_if_t<std::is_same_v<float, T> ||
                                     std::is_same_v<double, T> ||
                                     std::is_same_v<long double, T>, Value>
    make(T value) {
        Value result;
        result._type = Type::Float;
        result._isConstant = false;
        result.floatValue = static_cast<Float64>(value);
        return result;
    }

    template <typename T>
    [[nodiscard]]
    static typename std::enable_if_t<std::is_same_v<void*, T> ||
                                     std::is_same_v<nullptr_t, T>, Value>
    make() {
        Value result;
        result._type = Type::Pointer;
        result._isConstant = false;
        result.pointerValue = nullptr;
        return result;
    }

    template <typename T>
    [[nodiscard]]
    static typename std::enable_if_t<std::is_same_v<void*, T> ||
                                     std::is_same_v<nullptr_t, T>, Value>
    make(T value) {
        Value result;
        result._type = Type::Pointer;
        result._isConstant = false;
        result.pointerValue = static_cast<void*>(value);
        return result;
    }

    template <typename T>
    [[nodiscard]]
    static typename std::enable_if_t<std::is_same_v<bool, T>, Value>
    make() {
        Value result;
        result._type = Type::Boolean;
        result._isConstant = false;
        result.booleanValue = false;
        return result;
    }

    template <typename T>
    [[nodiscard]]
    static typename std::enable_if_t<std::is_same_v<bool, T>, Value>
    make(bool value) {
        Value result;
        result._type = Type::Boolean;
        result._isConstant = false;
        result.booleanValue = value;
        return result;
    }



    /* ===== Operators ===== */

public:
    friend std::ostream& operator<<(std::ostream& os, const Value& value) {
        switch (value._type) {
            case Type::Nil:
                os << "nil";
                break;

            case Type::Integer:
                os << value.integerValue;
                break;

            case Type::Float:
                os << value.floatValue;
                break;

            case Type::Boolean:
                os << (value.booleanValue ? "true" : "false");
                break;

            case Type::Pointer: {
                os << "0x"
                   << std::hex
                   << std::setw(2 * sizeof(void*))
                   << std::setfill('0')
                   << reinterpret_cast<uintptr_t>(value.pointerValue)
                   << std::dec;
            }
                break;
        }
        return os;
    }

};

} // Spark
