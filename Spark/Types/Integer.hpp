#pragma once

#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <type_traits>

namespace Spark {

struct Float;

struct Integer {

    /* ===== Data ===== */

private:
    int64_t _value;

public:
    [[nodiscard]]
    constexpr int64_t value() const { return _value; }



    /* ===== Constructors ===== */

public:
    constexpr Integer() : _value(0) { }
    constexpr Integer(const char value) = delete;
    constexpr Integer(const unsigned char value) = delete;
    constexpr Integer(const short value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(const unsigned short value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(const int value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(const unsigned int value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(const long value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(const unsigned long value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(const long long value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(const unsigned long long value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(const float value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(const double value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(const long double value) : _value(static_cast<int64_t>(value)) { }
    constexpr Integer(Float f);



    /* ===== Conversion Operators ===== */

public:
    explicit constexpr operator char() const { return static_cast<char>(_value); }
    explicit constexpr operator unsigned char() const { return static_cast<unsigned char>(_value); }
    explicit constexpr operator short() const { return static_cast<short>(_value); }
    explicit constexpr operator unsigned short() const { return static_cast<unsigned short>(_value); }
    explicit constexpr operator int() const { return static_cast<int>(_value); }
    explicit constexpr operator unsigned int() const { return static_cast<unsigned int>(_value); }
    explicit constexpr operator long() const { return static_cast<long>(_value); }
    explicit constexpr operator unsigned long() const { return static_cast<unsigned long>(_value); }
    explicit constexpr operator long long() const { return static_cast<long long>(_value); }
    explicit constexpr operator unsigned long long() const { return static_cast<unsigned long long>(_value); }
    explicit constexpr operator float() const { return static_cast<float>(_value); }
    explicit constexpr operator double() const { return static_cast<double>(_value); }
    explicit constexpr operator long double() const { return static_cast<long double>(_value); }



    /* ===== Operators ===== */

public:
    template <typename T>
    typename std::enable_if<std::is_same<T, short>::value ||
                            std::is_same<T, unsigned short>::value ||
                            std::is_same<T, int>::value ||
                            std::is_same<T, unsigned int>::value ||
                            std::is_same<T, long>::value ||
                            std::is_same<T, unsigned long>::value ||
                            std::is_same<T, long long>::value ||
                            std::is_same<T, unsigned long long>::value, Integer>::type
    constexpr operator+(const T other) const {
        return Integer{_value + static_cast<int64_t>(other)};
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, float>::value ||
                            std::is_same<T, double>::value ||
                            std::is_same<T, long double>::value, Float>::type
    constexpr operator+(const T other) const {
        return {_value + other};
    }

    constexpr Integer operator+(const Integer other) const {
        return {_value + other._value};
    }

    constexpr Float operator+(const Float other) const {
        return {static_cast<double>(_value) + other.value()};
    }

    constexpr Integer& operator++() {
        _value++;
        return *this;
    }

    constexpr const Integer operator++(int) {
        const Integer temp = *this;
        _value++;
        return temp;
    }

    constexpr Integer operator-(const Integer other) const {
        return {_value - other._value};
    }

    constexpr Integer& operator--() {
        _value--;
        return *this;
    }

    constexpr const Integer operator--(int) {
        const Integer temp = *this;
        _value--;
        return temp;
    }

    constexpr Integer operator*(const Integer other) const {
        return {_value * other._value};
    }

    constexpr Integer operator/(const Integer other) const {
        if (other._value == 0)
            // TODO: Consider to throw C++ exception or custom Spark exception type
            throw std::runtime_error("Division by zero.");
        return {_value / other._value};
    }

    constexpr Integer operator%(const Integer other) const {
        if (other._value == 0)
            throw std::runtime_error("Division by zero.");
        return {_value % other._value};
    }

    constexpr Integer operator&(const Integer other) const {
        return {_value & other._value};
    }

    constexpr Integer operator|(const Integer other) const {
        return {_value | other._value};
    }

    constexpr Integer operator^(const Integer other) const {
        return {_value ^ other._value};
    }

    constexpr Integer operator~() const {
        return {~_value};
    }

    constexpr bool operator==(const Integer other) const {
        return _value == other._value;
    }

    constexpr bool operator!=(const Integer other) const {
        return _value != other._value;
    }

    constexpr bool operator<(const Integer other) const {
        return _value < other._value;
    }

    constexpr bool operator>(const Integer other) const {
        return _value > other._value;
    }

    constexpr bool operator<=(const Integer other) const {
        return _value <= other._value;
    }

    constexpr bool operator>=(const Integer other) const {
        return _value >= other._value;
    }

    friend std::ostream& operator<<(std::ostream& os, const Integer integer) {
        os << integer._value;
        return os;
    }

};

} // Spark

#include "Integer.inl"
