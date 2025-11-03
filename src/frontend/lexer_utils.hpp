#pragma once

#include <string>
#include <string_view>

#include "parser.tab.hpp"

namespace Spark::FrontEnd {

using TokenType = yytoken_kind_t;

struct LexerState {
    size_t line;
    size_t column;
};

void handleNewline(LexerState& lstate) noexcept;

void consumeCharacters(LexerState& lstate, size_t n) noexcept;

TokenType makeToken(std::string_view lexeme, TokenType type, LexerState& lstate) noexcept;

TokenType makeWord(std::string_view lexeme, LexerState& lstate) noexcept;

} // Spark::Frontend
