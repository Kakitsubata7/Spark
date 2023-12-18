#pragma once

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



    /* ===== What ===== */

public:
    [[nodiscard]]
    const char* what() const {
        return data->message.c_str();
    }

};

} // Spark
