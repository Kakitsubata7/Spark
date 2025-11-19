#pragma once

#include <string>
#include <vector>

#include "lexer_error.hpp"
#include "token_buffer.hpp"

namespace Spark::FrontEnd {

/**
 * Represents additional information used by Flex (as `yyextra`).
 */
struct LexerState {
    /**
     * Current line number of the state.
     */
    size_t line = 1;

    /**
     * Current column number of the state.
     */
    size_t column = 1;

    /**
     * Pointer to the `std::istream` being processed by the state.
     */
    std::istream* stream = nullptr;

    /**
     * The opening/closing delimiter used by the currently processing string literal.
     */
    char stringDelimiter = '\0';

    /**
      * String buffer to store a currently processing token.
      */
    TokenBuffer tokenBuffer{1, 1};

    /**
     * Lexing errors encountered by the state.
     */
    std::vector<LexerError> errors;

    /**
     * Constructs a `LexerState` with the beginning line, column and the stream to read from.
     * @param line Beginning line.
     * @param column Beginning column.
     * @param stream Pointer to the stream to read from.
     */
    LexerState(size_t line, size_t column, std::istream* stream)
        : line(line), column(column), stream(stream) { }

    LexerState(const LexerState& other) = delete;
    LexerState& operator=(const LexerState& other) = delete;

    LexerState(LexerState&& other) = default;
    LexerState& operator=(LexerState&& other) = default;

    /**
     * Updates the state when a newline is encountered.
     */
    constexpr void whenNewline() noexcept {
        ++line;
        column = 1;
    }

    /**
     * Updates the state when @p n characters are consumed.
     * @param n Number of characters consumed.
     */
    constexpr void consume(size_t n) noexcept {
        column += n;
    }

    /**
     * Adds a new `LexerError` instance to the state.
     * @param error `LexerError` instance to add.
     */
    void addError(LexerError error) {
        errors.push_back(std::move(error));
    }

    /**
     * Constructs and adds a new `LexerError` instance to the state.
     * @param message Error message.
     * @param line Line number where the error occurred.
     * @param column Column number where the error occurred.
     */
    void addError(std::string_view message, size_t line, size_t column) {
        errors.emplace_back(std::string(message), line, column);
    }
};

} // Spark::FrontEnd
