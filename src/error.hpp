#pragma once

#include <string>

namespace Spark {

class Error {
private:
    std::string _message;

public:
    explicit Error(std::string message) noexcept : _message(message) { }

    [[nodiscard]]
    constexpr const std::string& message() const noexcept { return _message; }
};

} // Spark
