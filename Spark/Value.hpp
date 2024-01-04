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
#include "Exception.hpp"
#include "Float64.hpp"
#include "GC/GCPtr.hpp"
#include "Int64.hpp"
#include "Type.hpp"

namespace Spark {

class Value {

    /* ===== Data ===== */

private:
    Type _type;

    union {
        Int64 integerValue;
        Float64 floatValue;
        Bool8 booleanValue;
        void* pointerValue;
        Type typeValue;
        GCPtr<Value> boxPtr;
        GCPtr<std::string> stringPtr;
        GCPtr<std::vector<Value>> arrayPtr;
        GCPtr<std::unordered_set<Value>> setPtr;
        GCPtr<std::unordered_map<Value, Value>> mapPtr;
        GCPtr<std::unordered_map<std::string, Value>> objectPtr;
        GCPtr<void*> functionPtr;
        GCPtr<Exception> exceptionPtr;
        GCPtr<void*> threadPtr;
        GCPtr<void*> promisePtr;
    };

public:
    [[nodiscard]]
    constexpr Type type() const { return _type; }

    [[nodiscard]]
    constexpr bool isReferenceType() const {
        switch (_type) {
            case Type::Box:
            case Type::String:
            case Type::Array:
            case Type::Set:
            case Type::Map:
            case Type::Object:
            case Type::Function:
            case Type::Exception:
            case Type::Thread:
            case Type::Promise:
                return true;

            default:
                return false;
        }
    }



    /* ===== Constructor ===== */

public:
    explicit constexpr Value() : _type(Type::Nil), pointerValue(nullptr) { }



    /* ===== Destructor ===== */

public:
    ~Value() {
        switch (_type) {
            case Type::Box:
                boxPtr.~GCPtr<Value>();
                break;

            case Type::String:
                stringPtr.~GCPtr<std::string>();
                break;

            case Type::Array:
                arrayPtr.~GCPtr<std::vector<Value>>();
                break;

            case Type::Set:
                setPtr.~GCPtr<std::unordered_set<Value>>();
                break;

            case Type::Map:
                mapPtr.~GCPtr<std::unordered_map<Value, Value>>();
                break;

            case Type::Object:
                objectPtr.~GCPtr<std::unordered_map<std::string, Value>>();
                break;

            case Type::Function:
                functionPtr.~GCPtr<void*>();
                break;

            case Type::Thread:
                threadPtr.~GCPtr<void*>();
                break;

            case Type::Promise:
                promisePtr.~GCPtr<void*>();
                break;

            default:
                break;
        }
    }



    /* ===== Copying ===== */

public:
    Value(const Value& other) : _type(other._type) {
        copyFrom(other);
    }

    Value& operator=(const Value& other) {
        if (this != &other)
            copyFrom(other);
        return *this;
    }

private:
    void copyFrom(const Value& other) {
        switch (_type) {
            case Type::Integer:
                integerValue = other.integerValue;
                break;

            case Type::Float:
                floatValue = other.floatValue;
                break;

            case Type::Boolean:
                booleanValue = other.booleanValue;
                break;

            case Type::Pointer:
                pointerValue = other.pointerValue;
                break;

            case Type::Type:
                typeValue = other.typeValue;
                break;

            case Type::Box:
                boxPtr = other.boxPtr;
                break;

            case Type::String:
                stringPtr = other.stringPtr;
                break;

            case Type::Array:
                arrayPtr = other.arrayPtr;
                break;

            case Type::Set:
                setPtr = other.setPtr;
                break;

            case Type::Map:
                mapPtr = other.mapPtr;
                break;

            case Type::Object:
                objectPtr = other.objectPtr;
                break;

            case Type::Function:
                functionPtr = other.functionPtr;
                break;

            case Type::Thread:
                threadPtr = other.threadPtr;
                break;

            case Type::Promise:
                promisePtr = other.promisePtr;
                break;

            default:
                break;
        }
    }



    /* ===== Move Semantics ===== */

public:
    Value(Value&& other) noexcept : _type(other._type) {
        moveFrom(std::move(other));
    }

    Value& operator=(Value&& other) noexcept {
        if (this != &other) {
            _type = other._type;
            moveFrom(std::move(other));
        }
        return *this;
    }

private:
    void moveFrom(Value&& other) noexcept {
        switch (_type) {
            case Type::Integer:
                integerValue = other.integerValue;
                break;

            case Type::Float:
                floatValue = other.floatValue;
                break;

            case Type::Boolean:
                booleanValue = other.booleanValue;
                break;

            case Type::Pointer:
                pointerValue = other.pointerValue;
                break;

            case Type::Type:
                typeValue = other.typeValue;
                break;

            case Type::Box:
                boxPtr = other.boxPtr;
                other.boxPtr.invalidate();
                break;

            case Type::String:
                stringPtr = other.stringPtr;
                other.stringPtr.invalidate();
                break;

            case Type::Array:
                arrayPtr = other.arrayPtr;
                other.arrayPtr.invalidate();
                break;

            case Type::Set:
                setPtr = other.setPtr;
                other.setPtr.invalidate();
                break;

            case Type::Map:
                mapPtr = other.mapPtr;
                other.mapPtr.invalidate();
                break;

            case Type::Object:
                objectPtr = other.objectPtr;
                other.objectPtr.invalidate();
                break;

            case Type::Function:
                functionPtr = other.functionPtr;
                other.functionPtr.invalidate();
                break;

            case Type::Exception:
                exceptionPtr = other.exceptionPtr;
                other.exceptionPtr.invalidate();
                break;

            case Type::Thread:
                threadPtr = other.threadPtr;
                other.threadPtr.invalidate();
                break;

            case Type::Promise:
                promisePtr = other.promisePtr;
                other.promisePtr.invalidate();
                break;

            default:
                break;
        }
    }



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



    /* ===== Factory Methods ===== */

public:
    [[nodiscard]]
    static Value makeNil() {
        Value value;
        value._type = Type::Nil;
        return value;
    }

    template <typename T, typename std::enable_if<is_integer_v<T>, int>::type = 0>
    [[nodiscard]]
    static Value makeInteger(T integer = 0) {
        Value value;
        value._type = Type::Integer;
        value.integerValue = static_cast<Int64>(integer);
        return value;
    }

    template <typename T, typename std::enable_if<is_float_v<T>, int>::type = 0>
    [[nodiscard]]
    static Value makeFloat(T f = 0.0) {
        Value value;
        value._type = Type::Float;
        value.floatValue = static_cast<Float64>(f);
        return value;
    }

    template <typename T, typename std::enable_if<is_boolean_v<T>, int>::type = 0>
    [[nodiscard]]
    static Value makeBoolean(T boolean = false) {
        Value value;
        value._type = Type::Boolean;
        value.booleanValue = static_cast<Bool8>(boolean);
        return value;
    }

    template <typename T, typename std::enable_if<is_pointer_v<T>, int>::type = 0>
    [[nodiscard]]
    static Value makePointer(T pointer = nullptr) {
        Value value;
        value._type = Type::Pointer;
        value.pointerValue = static_cast<void*>(pointer);
        return value;
    }

    [[nodiscard]]
    static Value makeString(GC& gc, const char* str = "") {
        Value value;
        value._type = Type::String;
        value.stringPtr = gc.make<std::string>(str);
        return value;
    }

    [[nodiscard]]
    static Value makeString(GC& gc, const std::string& str) {
        Value value;
        value._type = Type::String;
        value.stringPtr = gc.make<std::string>(str);
        return value;
    }



    /* ===== Assignment Operator ===== */

public:
    template <typename T, typename std::enable_if<is_integer_v<T>, int>::type = 0>
    Value& operator=(const T integer) {
        this->_type = Type::Integer;
        this->integerValue = static_cast<Int64>(integer);
        return *this;
    }

    template <typename T, typename std::enable_if<is_float_v<T>, int>::type = 0>
    Value& operator=(const T f) {
        this->_type = Type::Float;
        this->floatValue = static_cast<Float64>(f);
        return *this;
    }

    template <typename T, typename std::enable_if<is_boolean_v<T>, int>::type = 0>
    Value& operator=(const T boolean) {
        this->_type = Type::Boolean;
        this->booleanValue = static_cast<Bool8>(boolean);
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

            case Type::Box:
                os << *(value.boxPtr);
                break;

            case Type::String:
                os << *(value.stringPtr);
                break;

            case Type::Array: {
                os << '[';
                GCPtr<std::vector<Value>> arrayPtr = value.arrayPtr;
                size_t size = value.arrayPtr->size();
                for (size_t i = 0; i < size; i++) {
                    os << arrayPtr->operator[](i);
                    if (i < size - 1)
                        os << ", ";
                }
                os << ']';
            }
                break;

            case Type::Set: {
                os << '{';
                GCPtr<std::unordered_set<Value>> setPtr = value.setPtr;
                for (auto it = setPtr->begin(); it != setPtr->end(); ) {
                    os << *it;
                    if (++it != setPtr->end())
                        os << ", ";
                }
                os << '}';
            }
                break;

            case Type::Map: {
                os << '{';
                GCPtr<std::unordered_map<Value, Value>> mapPtr = value.mapPtr;
                for (auto it = mapPtr->begin(); it != mapPtr->end(); ) {
                    os << it->first << ": " << it->second;
                    if (++it != mapPtr->end()) {
                        os << ", ";
                    }
                }
                os << '}';
            }
                break;

            case Type::Object:
                os << "<Object: "
                   << value.objectPtr
                   << '>';
                break;

            case Type::Function:
                os << "<Function: "
                   << value.functionPtr
                   << '>';
                break;

            case Type::Exception:
                os << *(value.exceptionPtr);
                break;

            case Type::Thread:
                os << "<Thread: "
                   << value.threadPtr
                   << '>';
                break;

            case Type::Promise:
                os << "<Promise: "
                   << value.threadPtr
                   << '>';
                break;
        }
        return os;
    }

};

} // Spark
