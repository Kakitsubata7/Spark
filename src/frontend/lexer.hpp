#pragma once

#include <utility>
#include <vector>

#include "lexer/lexer_state.hpp"
#include "lexer/lexer_utils.hpp"
#include "lexer/token.hpp"
#include "lexer/token_buffer.hpp"
#include "lexer/token_type.hpp"
#include "lexer/token_value.hpp"
#include "utils/error.hpp"
#include "utils/result.hpp"
#include "source_buffer.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a wrapper to Flex's lexer logics.
 */
class Lexer {
private:
    yyscan_t _scanner;
    LexerState _lstate;

public:
    explicit Lexer(std::istream& stream);
    Lexer(std::istream& stream, SourceBuffer& srcbuf);
    ~Lexer();

    Lexer(const Lexer& other) = delete;
    Lexer& operator=(const Lexer& other) = delete;

    Lexer(Lexer&& other) noexcept;
    Lexer& operator=(Lexer&& other) noexcept;

    /**
     * Gets whether the lexer produced any error during lexing.
     * @return true if the lexer has at least an error, false otherwise.
     */
    [[nodiscard]]
    bool hasError() const noexcept {
        return !_lstate.errors().empty();
    }

    /**
     * Retrieves the errors occurred during lexing.
     * @return Errors occurred during lexing.
     */
    [[nodiscard]]
    const std::vector<Error>& errors() const noexcept {
        return _lstate.errors();
    }

    /**
     * Lexes a single token.
     * @return Lexed token.
     */
    Token lex();

    /**
     * Lexes all tokens till the end of the stream.
     * @return Lexed tokens.
     */
    std::vector<Token> lexAll();

    /**
     * Lexes all tokens from a stream, returns the result and errors occurred.
     * @param stream Stream to lex from.
     * @return Lexed tokens and errors occurred as a pair.
     */
    static std::pair<std::vector<Token>, std::vector<Error>> lexAll(std::istream& stream);

    /**
     * Switches the lexer's stream to a new stream.
     * @param stream New stream.
     */
    void switchStream(std::istream& stream) noexcept {
        _lstate.switchStream(stream);
    }

    /**
     * Clears the lexer.
     */
    void clear() noexcept {
        _lstate.clear();
    }

    friend class Parser;
};

} // Spark::FrontEnd
