#pragma once

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <string>
#include <type_traits>
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
    constexpr Value() : Value(false) { }

private:
    explicit constexpr Value(bool isConstant) : _type(Type::Nil), _isConstant(isConstant), pointerValue(nullptr) { }



    /* ===== Type Traits ===== */

private:
    template <typename T>
    inline static constexpr bool is_integer_v = std::disjunction_v<std::is_same<short, T>,
                                                                   std::is_same<unsigned short, T>,
                                                                   std::is_same<int, T>,
                                                                   std::is_same<unsigned int, T>,
                                                                   std::is_same<long, T>,
                                                                   std::is_same<unsigned long, T>,
                                                                   std::is_same<long long, T>,
                                                                   std::is_same<unsigned long long, T>>;

    template <typename T>
    inline static constexpr bool is_float_v = std::disjunction_v<std::is_same<float, T>,
                                                                 std::is_same<double, T>,
                                                                 std::is_same<long double, T>>;

    template <typename T>
    inline static constexpr bool is_boolean_v = std::disjunction_v<std::is_same<bool, T>,
                                                                   std::is_same<Bool8, T>>;

    template <typename T>
    inline static constexpr bool is_pointer_v = std::disjunction_v<std::is_same<void*, T>,
                                                                   std::is_same<std::nullptr_t, T>>;

    template <typename T>
    inline static constexpr bool is_type_v = std::is_same_v<Type, T>;

    template <typename T>
    inline static constexpr bool is_spark_type_v = is_integer_v<T> ||
                                                   is_float_v<T> ||
                                                   is_boolean_v<T> ||
                                                   is_pointer_v<T> ||
                                                   is_type_v<T>;

    template <typename T>
    using IntegerValue = typename std::enable_if_t<is_integer_v<T>, Value>;

    template <typename T>
    using FloatValue = typename std::enable_if_t<is_float_v<T>, Value>;

    template <typename T>
    using BooleanValue = typename std::enable_if_t<is_boolean_v<T>, Value>;

    template <typename T>
    using PointerValue = typename std::enable_if_t<is_pointer_v<T>, Value>;

    template <typename T>
    using TypeValue = typename std::enable_if_t<is_type_v<T>, Value>;



    /* ===== Factory Methods ===== */

private:
    template <typename T, typename std::enable_if_t<is_integer_v<T>, int> = 0>
    inline static void construct(Value& value, T integer, bool isConstant) {
        value._type = Type::Integer;
        value._isConstant = isConstant;
        value.integerValue = static_cast<Int64>(integer);
    }

    template <typename T, typename std::enable_if_t<is_float_v<T>, int> = 0>
    inline static void construct(Value& value, T f, bool isConstant) {
        value._type = Type::Float;
        value._isConstant = isConstant;
        value.floatValue = static_cast<Float64>(f);
    }

    template <typename T, typename std::enable_if_t<is_boolean_v<T>, int> = 0>
    inline static void construct(Value& value, T boolean, bool isConstant) {
        value._type = Type::Boolean;
        value._isConstant = isConstant;
        value.booleanValue = static_cast<Bool8>(boolean);
    }

    template <typename T, typename std::enable_if_t<is_pointer_v<T>, int> = 0>
    inline static void construct(Value& value, T pointer, bool isConstant) {
        value._type = Type::Pointer;
        value._isConstant = isConstant;
        value.pointerValue = static_cast<void*>(pointer);
    }

    template <typename T, typename std::enable_if_t<is_type_v<T>, int> = 0>
    inline static void construct(Value& value, T type, bool isConstant) {
        value._type = Type::Type;
        value._isConstant = isConstant;
        value.typeValue = static_cast<Type>(type);
    }

public:
    template <typename T>
    [[nodiscard]]
    static IntegerValue<T> make(T integer = 0) {
        Value value;
        construct<T>(value, integer, false);
        return value;
    }

    template <typename T>
    [[nodiscard]]
    static const IntegerValue<T> makeConstant(T integer = 0) {
        Value value;
        construct<T>(value, integer, true);
        return value;
    }

    template <typename T>
    [[nodiscard]]
    static FloatValue<T> make(T f = 0.0) {
        Value value;
        construct<T>(value, f, false);
        return value;
    }

    template <typename T>
    [[nodiscard]]
    static const FloatValue<T> makeConstant(T f = 0.0) {
        Value value;
        construct<T>(value, f, true);
        return value;
    }

    template <typename T>
    [[nodiscard]]
    static BooleanValue<T> make(T boolean = false) {
        Value value;
        construct<T>(value, boolean, false);
        return value;
    }

    template <typename T>
    [[nodiscard]]
    static const BooleanValue<T> makeConstant(T boolean = false) {
        Value value;
        construct<T>(value, boolean, true);
        return value;
    }

    template <typename T>
    [[nodiscard]]
    static PointerValue<T> make(T pointer = nullptr) {
        Value value;
        construct<T>(value, pointer, false);
        return value;
    }

    template <typename T>
    [[nodiscard]]
    static const PointerValue<T> makeConstant(T pointer = nullptr) {
        Value value;
        construct<T>(value, pointer, true);
        return value;
    }

    template <typename T>
    [[nodiscard]]
    static TypeValue<T> make(T type = Type::Nil) {
        Value value;
        construct<T>(value, type, false);
        return value;
    }

    template <typename T>
    [[nodiscard]]
    static const TypeValue<T> makeConstant(T type = Type::Nil) {
        Value value;
        construct<T>(value, type, true);
        return value;
    }



    /* ===== Assignment Operators ===== */

public:
    template <typename T, typename std::enable_if<is_spark_type_v<T>, int>::type = 0>
    Value& operator=(const T value) {
        construct<T>(*this, value, _isConstant);
        return *this;
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

            case Type::Type:
                os << value.typeValue;
                break;

            // TODO: Implement
            case Type::String:
                break;

            case Type::Array:
                break;

            case Type::Set:
                break;

            case Type::Map:
                break;

            case Type::Object:
                break;

            case Type::Function:
                break;

            case Type::Thread:
                break;

            case Type::Promise:
                break;
        }
        return os;
    }

};

} // Spark
