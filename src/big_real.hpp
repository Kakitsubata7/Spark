#pragma once

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace Spark {

class BigReal {
private:
    using RationalType = boost::multiprecision::cpp_rational;

    RationalType _value;

public:
    BigReal() = default;

    BigReal(int value) : _value{value} { }
    BigReal(unsigned int value) : _value{value} { }
    BigReal(long value) : _value{value} { }
    BigReal(unsigned long value) : _value{value} { }
    BigReal(long long value) : _value{value} { }
    BigReal(unsigned long long value) : _value{value} { }
    BigReal(float value) : _value{value} { }
    BigReal(double value) : _value{value} { }
    BigReal(long double value) : _value{value} { }

    explicit BigReal(RationalType value) : _value{std::move(value)} { }

    BigReal(const BigReal&) = default;
    BigReal& operator=(const BigReal&) = default;

    BigReal(BigReal&&) = default;
    BigReal& operator=(BigReal&&) = default;

    BigReal operator+() const {
        return *this;
    }

    BigReal operator-() const {
        return BigReal{-_value};
    }

    BigReal& operator+=(const BigReal& other) {
        _value += other._value;
        return *this;
    }

    BigReal& operator-=(const BigReal& other) {
        _value -= other._value;
        return *this;
    }

    BigReal& operator*=(const BigReal& other) {
        _value *= other._value;
        return *this;
    }

    BigReal& operator/=(const BigReal& other) {
        _value /= other._value;
        return *this;
    }

    friend BigReal operator+(BigReal lhs, const BigReal& rhs) {
        return lhs += rhs;
    }

    friend BigReal operator-(BigReal lhs, const BigReal& rhs) {
        return lhs -= rhs;
    }

    friend BigReal operator*(BigReal lhs, const BigReal& rhs) {
        return lhs *= rhs;
    }

    friend BigReal operator/(BigReal lhs, const BigReal& rhs) {
        return lhs /= rhs;
    }

    friend bool operator==(const BigReal&, const BigReal&) = default;

    friend auto operator<=>(const BigReal& lhs, const BigReal& rhs) {
        if (lhs._value < rhs._value) {
            return std::strong_ordering::less;
        }
        if (lhs._value > rhs._value) {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }

    friend std::ostream& operator<<(std::ostream&, const BigReal&);
};

inline std::ostream& operator<<(std::ostream& os, const BigReal& value) {
    os << value._value
              .convert_to<boost::multiprecision::cpp_dec_float_50>()
              .str(5, std::ios_base::fixed);
    return os;
}

} // Spark
