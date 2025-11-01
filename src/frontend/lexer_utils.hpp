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

TokenType makeWord(std::string_view sv, LexerState& state) noexcept;

TokenType makeInteger(std::string_view sv, LexerState& state) noexcept;

void whenNewline(LexerState& state);

void consumeCharacters(LexerState& state, size_t n);

} // Spark::Frontend
