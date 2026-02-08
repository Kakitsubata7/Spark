#pragma once

#include <vector>

#include "lexer/lexer_state.hpp"
#include "lexer/lexer_utils.hpp"
#include "lexer/token.hpp"
#include "lexer/token_buffer.hpp"
#include "lexer/token_type.hpp"
#include "lexer/token_value.hpp"
#include "source_buffer.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

struct LexResult {
    std::vector<Token> tokens;
    SourceBuffer srcbuf;
    Diagnostics diagnostics;

    LexResult() noexcept = default;
    LexResult(std::vector<Token> tokens, SourceBuffer srcbuf, Diagnostics diagnostics) noexcept
        : tokens(std::move(tokens)), srcbuf(std::move(srcbuf)), diagnostics(std::move(diagnostics)) { }
};

/**
 * Represents a wrapper to Flex's lexer logics.
 */
class Lexer {
private:
    yyscan_t _scanner;
    LexerState _lstate;

    SourceBuffer _srcbuf;

    Diagnostics _diagnostics{};

private:
    explicit Lexer(SourceBuffer srcbuf);

public:
    explicit Lexer(std::istream& stream);

    ~Lexer();

    Lexer(const Lexer& other) = delete;
    Lexer& operator=(const Lexer& other) = delete;

    Lexer(Lexer&& other) noexcept = delete;
    Lexer& operator=(Lexer&& other) noexcept = delete;

    /**
     * Retrieves the diagnotics occurred during lexing.
     * @return Diagnostics occurred during lexing.
     */
    [[nodiscard]]
    constexpr const Diagnostics& diagnostics() const { return _diagnostics; }

    [[nodiscard]]
    constexpr const SourceBuffer& srcbuf() const noexcept { return _srcbuf; }

    /**
     * Clears the lexer.
     */
    void clear() noexcept {
        _lstate.clear();
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
     * Lexes all tokens till the end of ths stream.
     * @param stream Input stream.
     * @return Lexing result.
     */
    static LexResult lexAll(std::istream& stream);

    friend class Parser;
};

} // Spark::FrontEnd
