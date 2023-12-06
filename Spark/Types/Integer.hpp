#pragma once

#include <cstdint>
#include <ostream>
#include <stdexcept>

namespace Spark {

    struct Integer {

        /* ===== Data ===== */

    private:
        int64_t value;



        /* ===== Constructors ===== */

    public:
        explicit constexpr Integer(const char value) : value(static_cast<int64_t>(value)) { }
        explicit constexpr Integer(const unsigned char value) : value(static_cast<int64_t>(value)) { }
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
        explicit constexpr operator char() const { return static_cast<char>(value); }
        explicit constexpr operator unsigned char() const { return static_cast<unsigned char>(value); }
        explicit constexpr operator short() const { return static_cast<short>(value); }
        explicit constexpr operator unsigned short() const { return static_cast<unsigned short>(value); }
        explicit constexpr operator int() const { return static_cast<int>(value); }
        explicit constexpr operator unsigned int() const { return static_cast<unsigned int>(value); }
        explicit constexpr operator long() const { return static_cast<long>(value); }
        explicit constexpr operator unsigned long() const { return static_cast<unsigned long>(value); }
        explicit constexpr operator long long() const { return static_cast<long long>(value); }
        explicit constexpr operator unsigned long long() const { return static_cast<unsigned long long>(value); }
        explicit constexpr operator float() const { return static_cast<float>(value); }
        explicit constexpr operator double() const { return static_cast<double>(value); }
        explicit constexpr operator long double() const { return static_cast<long double>(value); }



        /* ===== Operators ===== */

    public:
        constexpr Integer& operator=(const Integer other) {
            value = other.value;
            return *this;
        }

        constexpr Integer operator+(const Integer other) const {
            return {value + other.value};
        }

        constexpr Integer operator+(const short value) const {
            return {this->value + value};
        }

        constexpr Integer operator+(const unsigned short value) const {
            return {this->value + value};
        }

        constexpr Integer operator+(const int value) const {
            return {this->value + value};
        }

        constexpr Integer operator+(const unsigned int value) const {
            return {this->value + value};
        }

        constexpr Integer operator+(const long value) const {
            return {this->value + value};
        }

        constexpr Integer operator+(const unsigned long value) const {
            return {this->value + value};
        }

        constexpr Integer operator+(const long long value) const {
            return {this->value + value};
        }

        constexpr Integer operator+(const unsigned long long value) const {
            return {this->value + value};
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
                throw std::runtime_error("Division by zero.");
            return {value / other.value};
        }

        constexpr Integer operator%(const Integer other) const {
            if (other.value == 0)
                throw std::runtime_error("Division by zero.");
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
