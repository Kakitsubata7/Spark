#pragma once

#include <string>

namespace Spark {

class Error {
private:
    std::string _message;

public:
    explicit Error(std::string message) : _message(message) { }

    [[nodiscard]]
    constexpr const std::string& message() const { return _message; }
};

} // Spark
