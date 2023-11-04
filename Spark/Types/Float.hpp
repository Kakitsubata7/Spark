#pragma once

namespace Spark {

    struct Float {

        /* ===== Data ===== */

    private:
        double value;



        /* ===== Constructors ===== */

    public:
        constexpr Float() : value(0.0) { }
        constexpr Float(char value) : value(static_cast<double>(value)) { }
        constexpr Float(unsigned char value) : value(static_cast<double>(value)) { }
        constexpr Float(short value) : value(static_cast<double>(value)) { }
        constexpr Float(unsigned short value) : value(static_cast<double>(value)) { }
        constexpr Float(int value) : value(static_cast<double>(value)) { }
        constexpr Float(unsigned int value) : value(static_cast<double>(value)) { }
        constexpr Float(long value) : value(static_cast<double>(value)) { }
        constexpr Float(unsigned long value) : value(static_cast<double>(value)) { }
        constexpr Float(long long value) : value(static_cast<double>(value)) { }
        constexpr Float(unsigned long long value) : value(static_cast<double>(value)) { }
        constexpr Float(float value) : value(static_cast<double>(value)) { }
        constexpr Float(double value) : value(static_cast<double>(value)) { }
        constexpr Float(long double value) : value(static_cast<double>(value)) { }



        /* ===== Operators ===== */

    public:
        friend std::ostream& operator<<(std::ostream& os, const Float f) {
            os << f.value;
            return os;
        }

    };

} // Spark
