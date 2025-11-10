#pragma once

#include <string>
#include <utility>

namespace Spark {

class Error {
private:
    std::string _message;
    size_t _line;
    size_t _column;

public:
    Error(std::string message, size_t line, size_t column) noexcept :
        _message(std::move(message)), _line(line), _column(column) { }

    [[nodiscard]]
    constexpr const std::string& message() const noexcept { return _message; }

    [[nodiscard]]
    constexpr size_t line() const noexcept { return _line; }

    [[nodiscard]]
    constexpr size_t column() const noexcept { return _column; }
};

} // Spark
