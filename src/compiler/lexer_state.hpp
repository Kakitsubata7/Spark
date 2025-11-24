#pragma once

#include <string>
#include <vector>

#include "lexer_error.hpp"
#include "source_buffer.hpp"
#include "token_buffer.hpp"

namespace Spark::Compiler {

/**
 * Represents additional information used by Flex (as `yyextra`).
 */
struct LexerState {
    /**
     * Current line number of the state.
     */
    size_t lineno = 1;

    /**
     * Current column number of the state.
     */
    size_t columnno = 1;

    /**
     * Pointer to the `std::istream` being processed by the state.
     */
    std::istream* streamp = nullptr;

    SourceBuffer* srcbufp = nullptr;

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

    LexerState(const LexerState& other) = delete;
    LexerState& operator=(const LexerState& other) = delete;

    LexerState(LexerState&& other) = default;
    LexerState& operator=(LexerState&& other) = default;

    /**
     * Updates the state when a newline is encountered.
     */
    constexpr void whenNewline() noexcept {
        ++lineno;
        columnno = 1;
    }

    /**
     * Updates the state when @p n characters are consumed.
     * @param n Number of characters consumed.
     */
    constexpr void step(size_t n) noexcept {
        columnno += n;
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
     * @param lineno Line number where the error occurred.
     * @param columnno Column number where the error occurred.
     */
    void addError(std::string_view message, size_t lineno, size_t columnno) {
        errors.emplace_back(std::string(message), lineno, columnno);
    }
};

} // Spark::Compiler
