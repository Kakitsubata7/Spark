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
    template <typename T>
    [[nodiscard]]
    inline static IntegerValue<T> construct(T value, bool isConstant) {
        Value result;
        result._type = Type::Integer;
        result._isConstant = isConstant;
        result.integerValue = static_cast<Int64>(value);
        return result;
    }

    template <typename T>
    [[nodiscard]]
    inline static FloatValue<T> construct(T value, bool isConstant) {
        Value result;
        result._type = Type::Float;
        result._isConstant = isConstant;
        result.floatValue = static_cast<Float64>(value);
        return result;
    }

    template <typename T>
    [[nodiscard]]
    inline static BooleanValue<T> construct(T value, bool isConstant) {
        Value result;
        result._type = Type::Boolean;
        result._isConstant = isConstant;
        result.booleanValue = static_cast<Bool8>(value);
        return result;
    }

    template <typename T>
    [[nodiscard]]
    inline static PointerValue<T> construct(T value, bool isConstant) {
        Value result;
        result._type = Type::Pointer;
        result._isConstant = isConstant;
        result.pointerValue = static_cast<void*>(value);
        return result;
    }

    template <typename T>
    [[nodiscard]]
    inline static TypeValue<T> construct(T value, bool isConstant) {
        Value result;
        result._type = Type::Type;
        result._isConstant = isConstant;
        result.typeValue = static_cast<Type>(value);
        return result;
    }

public:
    [[nodiscard]]
    constexpr static Value makeNil() {
        return Value(false);
    }

    [[nodiscard]]
    constexpr static Value makeConstantNil() {
        return Value(true);
    }

    template <typename T>
    [[nodiscard]]
    static IntegerValue<T> make(T value = 0) {
        return construct<T>(value, false);
    }

    template <typename T>
    [[nodiscard]]
    static const IntegerValue<T> makeConstant(T value = 0) {
        return construct<T>(value, true);
    }

    template <typename T>
    [[nodiscard]]
    static FloatValue<T> make(T value = 0.0) {
        return construct<T>(value, false);
    }

    template <typename T>
    [[nodiscard]]
    static const FloatValue<T> makeConstant(T value = 0.0) {
        return construct<T>(value, true);
    }

    template <typename T>
    [[nodiscard]]
    static BooleanValue<T> make(T value = false) {
        return construct<T>(value, false);
    }

    template <typename T>
    [[nodiscard]]
    static const BooleanValue<T> makeConstant(T value = false) {
        return construct<T>(value, true);
    }

    template <typename T>
    [[nodiscard]]
    static PointerValue<T> make(T value = nullptr) {
        return construct<T>(value, false);
    }

    template <typename T>
    [[nodiscard]]
    static const PointerValue<T> makeConstant(T value = nullptr) {
        return construct<T>(value, true);
    }

    template <typename T>
    [[nodiscard]]
    static TypeValue<T> make(T value = nullptr) {
        return construct<T>(value, false);
    }

    template <typename T>
    [[nodiscard]]
    static const TypeValue<T> makeConstant(T value = nullptr) {
        return construct<T>(value, true);
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
