#pragma once

#include <string>
#include <string_view>

#include "utils/location.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a buffer used by `LexerState` to store a token currently being processed.
 */
class TokenBuffer {
private:
    Location _start;
    std::string _buffer;

public:
    /**
     * Constructs a `TokenBuffer` instance with the beginning location of the token being processed.
     * @param start Beginning location of the token being processed.
     */
    explicit TokenBuffer(Location start) : _start(start) { }

    /**
     * Constructs a `TokenBuffer` instance with the beginning line and column numbers of the token being processed.
     * @param lineno Beginning line number of the token being processed.
     * @param columnno Beginning column number of the token being processed.
     */
    TokenBuffer(size_t lineno, size_t columnno) : TokenBuffer(Location(lineno, columnno)) { }

    /**
     * Gets the beginning location of the token being processed.
     * @return Beginning location of the token being processed.
     */
    [[nodiscard]]
    Location start() const noexcept { return _start; }

    /**
     * Gets the beginning line number of the token being processed.
     * @return Line number of the token being processed.
     */
    [[nodiscard]]
    constexpr size_t lineno() const noexcept {
        return _start.lineno;
    }

    /**
     * Gets the beginning column number of the token being processed.
     * @return Column number of the token being processed.
     */
    [[nodiscard]]
    constexpr size_t columnno() const noexcept {
        return _start.columnno;
    }

    /**
     * Retrieves a string view to the token being processed.
     * @return String view to the token being processed.
     */
    [[nodiscard]]
    std::string_view view() const noexcept {
        return _buffer;
    }

    /**
     * Appends a string to the buffer.
     * @param sv String to append.
     */
    void append(std::string_view sv) {
        _buffer.append(sv);
    }

    /**
     * Appends a character to the buffer.
     * @param c Character to append.
     */
    void append(char c) {
        _buffer.push_back(c);
    }

    /**
     * Resets the buffer with a new set of beginning line and column numbers.
     * @param lineno Line number of the new token being processed.
     * @param columnno Column number of the new token being processed.
     */
    void reset(size_t lineno, size_t columnno) noexcept {
        _start = Location(lineno, columnno);
        _buffer.clear();
    }
};

} // Spark::FrontEnd
