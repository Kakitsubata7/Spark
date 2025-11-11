#pragma once

#include <string_view>

#include <boost/multiprecision/cpp_int.hpp>

namespace Spark {

class BigInt {
private:
    boost::multiprecision::cpp_int _value;

    explicit BigInt(boost::multiprecision::cpp_int cpp_int) noexcept : _value(std::move(cpp_int)) { }

public:
    BigInt() noexcept : _value(0) { }
    explicit BigInt(std::string_view sv) noexcept : _value(sv) { }

    BigInt operator+(const BigInt& rhs) const { return BigInt(_value + rhs._value); }
    BigInt operator-(const BigInt& rhs) const { return BigInt(_value - rhs._value); }
    BigInt operator*(const BigInt& rhs) const { return BigInt(_value * rhs._value); }
    BigInt operator/(const BigInt& rhs) const { return BigInt(_value / rhs._value); }
    BigInt operator%(const BigInt& rhs) const { return BigInt(_value % rhs._value); }

    BigInt& operator+=(const BigInt& rhs) { _value += rhs._value; return *this; }
    BigInt& operator-=(const BigInt& rhs) { _value -= rhs._value; return *this; }
    BigInt& operator*=(const BigInt& rhs) { _value *= rhs._value; return *this; }
    BigInt& operator/=(const BigInt& rhs) { _value /= rhs._value; return *this; }
    BigInt& operator%=(const BigInt& rhs) { _value %= rhs._value; return *this; }

    std::string str() const {
        return _value.str();
    }
};

} // Spark
