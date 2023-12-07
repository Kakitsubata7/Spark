#pragma once

#include <ostream>

namespace Spark {

struct Integer;

struct Float {

    /* ===== Data ===== */

private:
    double _value;

public:
    [[nodiscard]]
    constexpr double value() const { return _value; }



    /* ===== Constructors ===== */

public:
    constexpr Float() : _value(0.0) { }
    constexpr Float(const char value) = delete;
    constexpr Float(const unsigned char value) = delete;
    constexpr Float(const short value) : _value(static_cast<double>(value)) { }
    constexpr Float(const unsigned short value) : _value(static_cast<double>(value)) { }
    constexpr Float(const int value) : _value(static_cast<double>(value)) { }
    constexpr Float(const unsigned int value) : _value(static_cast<double>(value)) { }
    constexpr Float(const long value) : _value(static_cast<double>(value)) { }
    constexpr Float(const unsigned long value) : _value(static_cast<double>(value)) { }
    constexpr Float(const long long value) : _value(static_cast<double>(value)) { }
    constexpr Float(const unsigned long long value) : _value(static_cast<double>(value)) { }
    constexpr Float(const float value) : _value(static_cast<double>(value)) { }
    constexpr Float(const double value) : _value(static_cast<double>(value)) { }
    constexpr Float(const long double value) : _value(static_cast<double>(value)) { }
    constexpr Float(Integer integer);



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
    constexpr Float& operator=(const Float other) {
        _value = other._value;
        return *this;
    }

    constexpr Float operator+(const Float other) const {
        return {_value + other._value};
    }

    constexpr Float operator-(const Float other) const {
        return {_value - other._value};
    }

    constexpr Float operator*(const Float other) const {
        return {_value * other._value};
    }

    constexpr Float operator/(const Float other) const {
        // TODO: Check for division by zero
        return {_value / other._value};
    }

    friend std::ostream& operator<<(std::ostream& os, const Float f) {
        os << f._value;
        return os;
    }

};

} // Spark

#include "Float.inl"
