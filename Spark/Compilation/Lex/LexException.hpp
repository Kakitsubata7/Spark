#pragma once

#include <exception>
#include <string>

namespace Spark {

class LexException : public std::exception {

    /* ===== Data ===== */

private:
    std::string message;



    /* ===== Constructor ===== */

public:
    explicit LexException(const std::string& message) : message(message) { }
    explicit LexException(std::string&& message) : message(std::move(message)) { }



    /* ===== What ===== */

public:
    [[nodiscard]]
    const char* what() const noexcept override {
        return message.c_str();
    }

};

} // Spark
