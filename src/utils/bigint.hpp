#pragma once

#include <string_view>

#include <boost/multiprecision/cpp_int.hpp>

namespace Spark {

class BigInt {
private:
    boost::multiprecision::cpp_int _value;

    explicit BigInt(boost::multiprecision::cpp_int i) noexcept : _value(std::move(i)) { }

public:
    BigInt() noexcept : _value(0) { }
    explicit BigInt(std::string_view sv) : _value(sv) { }

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

    bool operator==(const BigInt& rhs) const noexcept { return _value == rhs._value; }
    bool operator!=(const BigInt& rhs) const noexcept { return _value != rhs._value; }
    bool operator<(const BigInt& rhs)  const noexcept { return _value <  rhs._value; }
    bool operator<=(const BigInt& rhs) const noexcept { return _value <= rhs._value; }
    bool operator>(const BigInt& rhs)  const noexcept { return _value >  rhs._value; }
    bool operator>=(const BigInt& rhs) const noexcept { return _value >= rhs._value; }

    [[nodiscard]]
    std::string str() const {
        return _value.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const BigInt& i) {
        os << i.str();
        return os;
    }
};

} // Spark
