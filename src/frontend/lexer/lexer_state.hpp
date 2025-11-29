#pragma once

#include <string>
#include <vector>

#include "frontend/source_buffer.hpp"
#include "token_buffer.hpp"
#include "utils/error.hpp"

namespace Spark::FrontEnd {

/**
 * Represents the lexer state used by Flex (as `yyextra`).
 */
class LexerState {
private:
    size_t _lineno;
    size_t _columnno;
    SourceBuffer* _srcbufp;
    TokenBuffer _tokbuf;
    char _strDelim = '\0';
    std::vector<Error> _errors;

public:
    [[nodiscard]]
    constexpr size_t lineno() const noexcept { return _lineno; }

    [[nodiscard]]
    constexpr size_t columnno() const noexcept { return _columnno; }

    [[nodiscard]]
    constexpr SourceBuffer& srcbuf() const noexcept { return *_srcbufp; }

    [[nodiscard]]
    constexpr TokenBuffer& tokbuf() noexcept { return _tokbuf; }

    [[nodiscard]]
    constexpr char strDelim() const noexcept { return _strDelim; }

    [[nodiscard]]
    constexpr const std::vector<Error>& errors() const noexcept { return _errors; }

    LexerState(SourceBuffer& srcbuf, size_t lineno, size_t columnno)
        : _lineno(lineno), _columnno(columnno), _srcbufp(&srcbuf),
          _tokbuf(lineno, columnno) { }

    LexerState(const LexerState& other) = delete;
    LexerState& operator=(const LexerState& other) = delete;

    LexerState(LexerState&& other) = default;
    LexerState& operator=(LexerState&& other) = default;

    /**
     * Updates the state when a newline is encountered.
     */
    constexpr void whenNewline() noexcept {
        ++_lineno;
        _columnno = 1;
    }

    /**
     * Updates the state when @p n characters are advanced.
     * @param n Number of characters advanced.
     */
    constexpr void advance(size_t n) noexcept {
        _columnno += n;
    }

    /**
     * Sets the string delimiter.
     * @param strDelim New string delimiter.
     */
    constexpr void setStrDelim(char strDelim) noexcept {
        _strDelim = strDelim;
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

    /**
     * Clears the lexer state.
     */
    void clear() {
        _srcbufp->clear();
        _tokbuf.reset(1, 1);
    }
};

} // Spark::FrontEnd
