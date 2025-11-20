#pragma once

#include <string>
#include <string_view>

namespace Spark::Compiler {

/**
 * Represents a buffer used by `LexerState` to store a token currently being processed.
 */
class TokenBuffer {
private:
    size_t _line;
    size_t _column;
    std::string _buffer;

public:
    /**
     * Constructs a `TokenBuffer` instance with the beginning line and column numbers of the token being processed.
     * @param line Beginning line number of the token being processed.
     * @param column Beginning column number of the token being processed.
     */
    TokenBuffer(size_t line, size_t column) : _line(line), _column(column) { }

    /**
     * Gets the beginning line number of the token being processed.
     * @return Line number of the token being processed.
     */
    [[nodiscard]]
    constexpr size_t line() const noexcept {
        return _line;
    }

    /**
     * Gets the beginning column number of the token being processed.
     * @return Column number of the token being processed.
     */
    [[nodiscard]]
    constexpr size_t column() const noexcept {
        return _column;
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
     * @param line Line number of the new token being processed.
     * @param column Column number of the new token being processed.
     */
    void reset(size_t line, size_t column) noexcept {
        _line = line;
        _column = column;
        _buffer.clear();
    }
};

} // Spark::Compiler
