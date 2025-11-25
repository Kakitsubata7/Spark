#pragma once

#include <optional>
#include <string_view>
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
    std::optional<std::string> _filename;

    yyscan_t _scanner;
    LexerState _lstate;

public:
    explicit Lexer(std::istream& stream,
                   std::optional<std::string_view> filename = std::nullopt,
                   SourceBuffer& srcbuf = NullSourceBuffer::instance());
    ~Lexer();

    Lexer(const Lexer& other) = delete;
    Lexer& operator=(const Lexer& other) = delete;

    Lexer(Lexer&& other) noexcept;
    Lexer& operator=(Lexer&& other) noexcept;

    [[nodiscard]]
    constexpr const std::optional<std::string>& filename() const noexcept { return _filename; }

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
