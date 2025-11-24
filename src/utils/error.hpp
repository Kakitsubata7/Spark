#pragma once

#include <string>

#include "location.hpp"

namespace Spark {

/**
 * Represents an error occurred from Spark's components.
 */
struct Error {
    std::string message;
    Location start;
    Location end;

    Error() = default;

    /**
     * Constructs an `Error` instance with message and locations of the error.
     * @param message Error message.
     * @param start Start location.
     * @param end End location.
     */
    Error(std::string message, Location start, Location end) noexcept
        : message(std::move(message)), start(start), end(end) { }

    /**
     * Constructs an `Error` instance with message and locations of the error.
     * @param message Error message.
     * @param sLineno Start line number.
     * @param sColumnno Start column number.
     * @param eLineno End line number.
     * @param eColumnno End column number.
     */
    Error(std::string message, size_t sLineno, size_t sColumnno, size_t eLineno, size_t eColumnno) noexcept
        : message(std::move(message)), start(sLineno, sColumnno), end(eLineno, eColumnno) { }
};

} // Spark
