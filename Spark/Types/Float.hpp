#pragma once

namespace Spark {

    struct Float {

        /* ===== Data ===== */

    private:
        double value;



        /* ===== Constructors ===== */

    public:
        explicit constexpr Float(const char value) : value(static_cast<double>(value)) { }
        explicit constexpr Float(const unsigned char value) : value(static_cast<double>(value)) { }
        constexpr Float(const short value) : value(static_cast<double>(value)) { }
        constexpr Float(const unsigned short value) : value(static_cast<double>(value)) { }
        constexpr Float(const int value) : value(static_cast<double>(value)) { }
        constexpr Float(const unsigned int value) : value(static_cast<double>(value)) { }
        constexpr Float(const long value) : value(static_cast<double>(value)) { }
        constexpr Float(const unsigned long value) : value(static_cast<double>(value)) { }
        constexpr Float(const long long value) : value(static_cast<double>(value)) { }
        constexpr Float(const unsigned long long value) : value(static_cast<double>(value)) { }
        constexpr Float(const float value) : value(static_cast<double>(value)) { }
        constexpr Float(const double value) : value(static_cast<double>(value)) { }
        constexpr Float(const long double value) : value(static_cast<double>(value)) { }



        /* ===== Conversion Operators ===== */

    public:
        constexpr operator float() const { return static_cast<float>(value); }
        constexpr operator double() const { return static_cast<double>(value); }
        constexpr operator long double() const { return static_cast<long double>(value); }



        /* ===== Operators ===== */

    public:
        constexpr Float& operator=(const Float other) {
            value = other.value;
            return *this;
        }

        constexpr Float operator+(const Float other) const {
            return {value + other.value};
        }

        constexpr Float operator-(const Float other) const {
            return {value - other.value};
        }

        constexpr Float operator*(const Float other) const {
            return {value * other.value};
        }

        constexpr Float operator/(const Float other) const {
            // TODO: Check for division by zero
            return {value / other.value};
        }

        friend std::ostream& operator<<(std::ostream& os, const Float f) {
            os << f.value;
            return os;
        }

    };

} // Spark
