#pragma once

#include <ostream>

#include <boost/multiprecision/cpp_int.hpp>

namespace Spark {

class BigInt {
private:
    using IntType = boost::multiprecision::cpp_int;

    IntType _value;

public:
    BigInt() = default;

    BigInt(int value) : _value{value} { }
    BigInt(unsigned int value) : _value{value} { }
    BigInt(long value) : _value{value} { }
    BigInt(unsigned long value) : _value{value} { }
    BigInt(long long value) : _value{value} { }
    BigInt(unsigned long long value) : _value{value} { }

    explicit BigInt(IntType value) : _value{std::move(value)} { }

    BigInt(const BigInt&) = default;
    BigInt& operator=(const BigInt&) = default;

    BigInt(BigInt&&) = default;
    BigInt& operator=(BigInt&&) = default;

    BigInt operator+() const {
        return *this;
    }

    BigInt operator-() const {
        return BigInt{-_value};
    }

    BigInt& operator+=(const BigInt& other) {
        _value += other._value;
        return *this;
    }

    BigInt& operator-=(const BigInt& other) {
        _value -= other._value;
        return *this;
    }

    BigInt& operator*=(const BigInt& other) {
        _value *= other._value;
        return *this;
    }

    BigInt& operator/=(const BigInt& other) {
        _value /= other._value;
        return *this;
    }

    BigInt& operator%=(const BigInt& other) {
        _value %= other._value;
        return *this;
    }

    friend BigInt operator+(BigInt lhs, const BigInt& rhs) {
        return lhs += rhs;
    }

    friend BigInt operator-(BigInt lhs, const BigInt& rhs) {
        return lhs -= rhs;
    }

    friend BigInt operator*(BigInt lhs, const BigInt& rhs) {
        return lhs *= rhs;
    }

    friend BigInt operator/(BigInt lhs, const BigInt& rhs) {
        return lhs /= rhs;
    }

    friend BigInt operator%(BigInt lhs, const BigInt& rhs) {
        return lhs %= rhs;
    }

    friend bool operator==(const BigInt&, const BigInt&) = default;

    friend auto operator<=>(const BigInt& lhs, const BigInt& rhs) {
        if (lhs._value < rhs._value) {
            return std::strong_ordering::less;
        }
        if (lhs._value > rhs._value) {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }

    friend std::ostream& operator<<(std::ostream&, const BigInt&);
};

inline std::ostream& operator<<(std::ostream& os, const BigInt& value) {
    os << value._value;
    return os;
}

}
