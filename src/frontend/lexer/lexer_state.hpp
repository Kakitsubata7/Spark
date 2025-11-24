#pragma once

#include <string>
#include <vector>

#include "frontend/source_buffer.hpp"
#include "token_buffer.hpp"
#include "utils/error.hpp"

namespace Spark::FrontEnd {

class LexerState2 {
private:
    size_t _lineno;
    size_t _columnno;
    std::istream* _streamp;
    SourceBuffer* _srcbufp;
    TokenBuffer _tokbuf;
    std::vector<Error> _errors;

public:
    char _strDelim = '\0';

    [[nodiscard]]
    constexpr std::istream& stream() noexcept { return *_streamp; }

    [[nodiscard]]
    constexpr const std::istream& stream() const noexcept { return *_streamp; }

    [[nodiscard]]
    constexpr TokenBuffer& tokbuf() noexcept { return _tokbuf; }

    [[nodiscard]]
    constexpr const std::vector<Error>& errors() const noexcept { return _errors; }

    LexerState2(std::istream& streamp, SourceBuffer srcbufp, size_t lineno, size_t columnno)
        : _lineno(lineno), _columnno(columnno), _streamp(&streamp), _srcbufp(&srcbufp),
          _tokbuf(lineno, columnno) { }

    // LexerState2(std::istream* streamp, size_t lineno, size_t columnno)
    //     : LexerState2(streamp, nullptr, lineno, columnno) { }

    LexerState2(const LexerState2& other) = delete;
    LexerState2& operator=(const LexerState2& other) = delete;

    LexerState2(LexerState2&& other) = default;
    LexerState2& operator=(LexerState2&& other) = default;

    void switchStream(std::istream& stream) noexcept {
        _streamp = &stream;
    }

    /**
      * Adds a new error to the state.
      * @param error `Error` instance to add.
      */
    void addError(Error error) noexcept {
        _errors.push_back(std::move(error));
    }

    /**
     * Constructs and adds a new error to the state.
     * @param message Error message.
     * @param start Start location where the error occurred.
     * @param end End location where the error occurred.
     */
    void addError(std::string_view message, Location start, Location end) {
        _errors.emplace_back(std::string(message), start, end);
    }
};

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
    std::vector<Error> errors;

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
    void addError(Error error) noexcept {
        errors.push_back(std::move(error));
    }

    /**
     * Constructs and adds a new `LexerError` instance to the state.
     * @param message Error message.
     * @param start Start location of the error.
     * @param end End location of the error.
     */
    void addError(std::string_view message, Location start, Location end) {
        errors.emplace_back(std::string(message), start, end);
    }
};

} // Spark::FrontEnd
