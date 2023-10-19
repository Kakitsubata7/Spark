#pragma once

#include <memory>
#include <string>

#include "Integer.hpp"

namespace Spark {

    class String {

        /* ===== Constructors ===== */

    public:
        explicit String(char c) : strPtr(std::make_unique<std::string>(1, c)) { }

        explicit String(const char* str) : strPtr(std::make_unique<std::string>(str)) { }

        explicit String(const std::string& str) : strPtr(std::make_unique<std::string>(str)) { }



        /* ===== Metadata ===== */

    private:
        std::unique_ptr<std::string> strPtr;

    public:
        [[nodiscard]]
        std::string& getString() const {
            return *strPtr;
        }



        /* ===== Operations ===== */

    public:
        [[nodiscard]]
        Integer length() const {
            return static_cast<Integer>(strPtr->length());
        }

        String operator[](Integer index) {
            return String((*strPtr)[index]);
        }

    };

} // Spark
