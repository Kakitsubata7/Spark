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
    constexpr Float(Integer integer);
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



    /* ===== Conversion Operators ===== */

public:
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

#include "Integer.hpp"

namespace Spark {

    constexpr Float::Float(Integer integer) : _value(static_cast<double>(integer)) { }

    constexpr Float::operator Integer() const { return {static_cast<double>(_value)}; }

}
