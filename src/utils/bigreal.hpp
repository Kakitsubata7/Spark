#pragma once

#include <string_view>

#include <boost/multiprecision/cpp_int.hpp>

namespace Spark {

class BigReal {
private:
    boost::multiprecision::cpp_rational _value;

    explicit BigReal(boost::multiprecision::cpp_rational r) noexcept : _value(std::move(r)) { }

public:
    BigReal() noexcept : _value(0) { }
    explicit BigReal(std::string_view sv) : _value(sv) { }

    BigReal operator+(const BigReal& rhs) const { return BigReal(_value + rhs._value); }
    BigReal operator-(const BigReal& rhs) const { return BigReal(_value - rhs._value); }
    BigReal operator*(const BigReal& rhs) const { return BigReal(_value * rhs._value); }
    BigReal operator/(const BigReal& rhs) const { return BigReal(_value / rhs._value); }

    BigReal& operator+=(const BigReal& rhs) { _value += rhs._value; return *this; }
    BigReal& operator-=(const BigReal& rhs) { _value -= rhs._value; return *this; }
    BigReal& operator*=(const BigReal& rhs) { _value *= rhs._value; return *this; }
    BigReal& operator/=(const BigReal& rhs) { _value /= rhs._value; return *this; }

    bool operator==(const BigReal& rhs) const noexcept { return _value == rhs._value; }
    bool operator!=(const BigReal& rhs) const noexcept { return _value != rhs._value; }
    bool operator<(const BigReal& rhs)  const noexcept { return _value <  rhs._value; }
    bool operator<=(const BigReal& rhs) const noexcept { return _value <= rhs._value; }
    bool operator>(const BigReal& rhs)  const noexcept { return _value >  rhs._value; }
    bool operator>=(const BigReal& rhs) const noexcept { return _value >= rhs._value; }

    [[nodiscard]]
    std::string str() const {
        return _value.str();
    }
};

} // Spark
