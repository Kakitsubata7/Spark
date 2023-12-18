#pragma once

#include <ostream>
#include <stdexcept>
#include <string>

namespace Spark {

class Exception {

    /* ===== Constructors & Destructor ===== */

public:
    explicit Exception(const char* message, const char* exceptionType = "Exception") {
        data = new Data();
        data->message = message;
        data->exceptionType = exceptionType;
    }

    ~Exception() {
        delete data;
    }



    /* ===== Data ===== */

private:
    struct Data {
        std::string message;
        std::string exceptionType;
    }* data;

public:
    const std::string& message() const {
        return data->message;
    }

    const std::string exceptionType() const {
        return data->exceptionType;
    }



    /* ===== What ===== */

public:
    [[nodiscard]]
    const char* what() const {
        return data->message.c_str();
    }



    /* ===== Operator ===== */

public:
    friend std::ostream& operator<<(std::ostream& os, const Exception& exception) {
        os << exception.exceptionType() << ": " << exception.message();
        return os;
    }

};

} // Spark
