#pragma once

#include <string>
#include <vector>

#include "frontend/source_buffer.hpp"
#include "frontend/source_reader.hpp"
#include "token_buffer.hpp"
#include "utils/error.hpp"

namespace Spark::FrontEnd {

/**
 * Represents the lexer state used by Flex (as `yyextra`).
 */
class LexerState {
private:
    SourceBuffer* _srcbufp;
    SourceReader _srcreader;

    TokenBuffer _tokbuf;

    std::vector<Error> _errors;

public:
    size_t line;
    size_t column;

    char strDelim = '\0';

    [[nodiscard]]
    constexpr TokenBuffer& tokbuf() noexcept { return _tokbuf; }

    [[nodiscard]]
    constexpr const std::vector<Error>& errors() const noexcept { return _errors; }

    LexerState(SourceBuffer& srcbuf, size_t lineno, size_t columnno)
        : _srcbufp(&srcbuf), _srcreader(srcbuf), _tokbuf(lineno, columnno), line(lineno), column(columnno) { }

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
     * Reads a string chunk with a max size of @p maxSize into @p buf. Returns the number of characters read.
     * @param buf Buffer to write into.
     * @param maxSize Max size of the buffer.
     * @return Number of characters read.
     */
    size_t readChunk(char* buf, size_t maxSize) noexcept {
        return _srcreader.readChunk(buf, maxSize);
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
