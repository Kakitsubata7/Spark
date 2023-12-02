#pragma once

#include <cstdint>
#include <ostream>

#include "../SparkException.hpp"

namespace Spark {

    struct Integer {

        /* ===== Data ===== */

    private:
        int64_t value;



        /* ===== Constructors ===== */

    public:
        constexpr Integer() : value(0) { }
        constexpr Integer(const char value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const unsigned char value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const short value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const unsigned short value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const int value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const unsigned int value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const long value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const unsigned long value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const long long value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const unsigned long long value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const float value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const double value) : value(static_cast<int64_t>(value)) { }
        constexpr Integer(const long double value) : value(static_cast<int64_t>(value)) { }



        /* ===== Conversion Operators ===== */

    public:
        constexpr operator char() const { return static_cast<char>(value); }
        constexpr operator unsigned char() const { return static_cast<unsigned char>(value); }
        constexpr operator short() const { return static_cast<short>(value); }
        constexpr operator unsigned short() const { return static_cast<unsigned short>(value); }
        constexpr operator int() const { return static_cast<int>(value); }
        constexpr operator unsigned int() const { return static_cast<unsigned int>(value); }
        constexpr operator long() const { return static_cast<long>(value); }
        constexpr operator unsigned long() const { return static_cast<unsigned long>(value); }
        constexpr operator long long() const { return static_cast<long long>(value); }
        constexpr operator unsigned long long() const { return static_cast<unsigned long long>(value); }
        constexpr operator float() const { return static_cast<float>(value); }
        constexpr operator double() const { return static_cast<double>(value); }
        constexpr operator long double() const { return static_cast<long double>(value); }



        /* ===== Operators ===== */

    public:
        constexpr Integer operator+(const Integer other) const {
            return {value + other.value};
        }

        constexpr Integer& operator++() {
            value++;
            return *this;
        }

        constexpr Integer& operator--() {
            value--;
            return *this;
        }

        constexpr const Integer operator++(int) {
            const Integer temp = *this;
            value++;
            return temp;
        }

        constexpr const Integer operator--(int) {
            const Integer temp = *this;
            value--;
            return temp;
        }

        constexpr Integer operator-(const Integer other) const {
            return {value - other.value};
        }

        constexpr Integer operator*(const Integer other) const {
            return {value * other.value};
        }

        constexpr Integer operator/(const Integer other) const {
            if (other.value == 0)
                // TODO: Consider to throw C++ exception or custom Spark exception type
                throw SparkException("Division by zero.");
            return {value / other.value};
        }

        constexpr Integer operator%(const Integer other) const {
            if (other.value == 0)
                throw SparkException("Division by zero.");
            return {value % other.value};
        }

        constexpr Integer operator&(const Integer other) const {
            return {value & other.value};
        }

        constexpr Integer operator|(const Integer other) const {
            return {value | other.value};
        }

        constexpr Integer operator^(const Integer other) const {
            return {value ^ other.value};
        }

        constexpr Integer operator~() const {
            return {~value};
        }

        constexpr bool operator==(const Integer other) const {
            return value == other.value;
        }

        constexpr bool operator!=(const Integer other) const {
            return value != other.value;
        }

        constexpr bool operator<(const Integer other) const {
            return value < other.value;
        }

        constexpr bool operator>(const Integer other) const {
            return value > other.value;
        }

        constexpr bool operator<=(const Integer other) const {
            return value <= other.value;
        }

        constexpr bool operator>=(const Integer other) const {
            return value >= other.value;
        }

        friend std::ostream& operator<<(std::ostream& os, const Integer integer) {
            os << integer.value;
            return os;
        }

    };

} // Spark
