#pragma once

#include <cstddef>
#include <optional>
#include <string>

#include "error.hpp"

namespace Spark {

template <typename T>
class Result {
private:
    std::optional<T> _value;
    std::optional<Error> _error;
    size_t _line;
    size_t _column;

public:
    Result(T value, size_t line, size_t column) : _value(value), _line(line), _column(column) { }
    Result(Error error, size_t line, size_t column) : _error(error), _line(line), _column(column) { }

    [[nodiscard]]
    constexpr size_t line() const noexcept { return _line; }

    [[nodiscard]]
    constexpr size_t column() const noexcept { return _column; }

    [[nodiscard]]
    constexpr bool hasError() const noexcept {
        return _error.has_value();
    }
};

} // Spark
