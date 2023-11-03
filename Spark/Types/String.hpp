#pragma once

#include <string>
#include <utility>

#include "Integer.hpp"

namespace Spark {

    class String {

        /* ===== Data ===== */

    private:
        std::string str;



        /* ===== Constructors ===== */

    public:
        String(const char* str) : str(str) { }
        String(std::string str) : str(std::move(str)) { }
        String() : String("") { }



        /* ===== Copying ===== */

    public:
        String(const String& other) : str(other.str) { }

        String& operator=(const String& other) {

            // Prevent self-assignment
            if (this != &other)
                str = other.str;

            return *this;
        }



        /* ===== Operations ===== */

    public:
        Integer length() {
            return Integer(str.size());
        }



        /* ===== Operators ===== */

    public:
        String operator+(const String& other) const {
            return {str + other.str};
        }

    };

} // Spark
