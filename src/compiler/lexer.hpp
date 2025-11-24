#pragma once

#include <vector>

#include "lexer_state.hpp"
#include "result.hpp"
#include "source_buffer.hpp"
#include "token.hpp"

namespace Spark::Compiler {

/**
 * Represents a wrapper to Flex's lexer logics.
 */
class Lexer {
private:
    yyscan_t _scanner;
    LexerState _lstate;

public:
    Lexer(std::istream& stream, SourceBuffer& srcbuf);
    explicit Lexer(std::istream& stream);
    ~Lexer();

    Lexer(const Lexer& other) = delete;
    Lexer& operator=(const Lexer& other) = delete;

    Lexer(Lexer&& other) noexcept;
    Lexer& operator=(Lexer&& other) noexcept;

    [[nodiscard]]
    bool hasError() const noexcept {
        return !_lstate.errors.empty();
    }

    [[nodiscard]]
    const std::vector<LexerError>& errors() const noexcept {
        return _lstate.errors;
    }

    Token lex();
    std::vector<Token> lexAll();

    static Result<std::vector<Token>, std::vector<LexerError>> lexAll(std::istream& stream);

    void switchStream(std::istream& stream) noexcept {
        _lstate.streamp = &stream;
    }

    void clear() noexcept {
        if (_lstate.srcbufp != nullptr) {
            _lstate.srcbufp->clear();
        }
        _lstate.tokenBuffer.reset(1, 1);
    }

    friend class Parser;
};

} // Spark::Compiler
