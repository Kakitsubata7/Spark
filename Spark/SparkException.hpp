#pragma once

#include <stdexcept>
#include <string>

namespace Spark {

    class SparkException : public std::exception {

        /* ===== Data ===== */

    private:
        const char* message;



        /* ===== Constructor ===== */

    public:
        explicit SparkException(const char* message) : message(message) { }



        /* ===== What ===== */

    public:
        [[nodiscard]]
        const char* what() const noexcept override {
            return message;
        }

    };

} // Spark
