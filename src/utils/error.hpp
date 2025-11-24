#pragma once

#include <string>

namespace Spark {

/**
 * Represents an error occurred from Spark's components.
 */
struct Error {
    std::string message;
    size_t line;
    size_t column;

    Error() = default;
    Error(std::string message, size_t line, size_t column) noexcept
        : message(std::move(message)), line(line), column(column) { }

    bool operator==(const Error& rhs) const noexcept {
        return message == rhs.message && line == rhs.line && column == rhs.column;
    }

    bool operator!=(const Error& rhs) const noexcept {
        return *this == rhs;
    }
};

} // Spark
