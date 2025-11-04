#pragma once

#include <string_view>

#include "lexer_state.hpp"
#include "lexer_utils.hpp"
#include "token_type.hpp"
#include "token_value.hpp"

namespace Spark::FrontEnd {

void handleNewline(LexerState& lstate) noexcept;

void consumeCharacters(size_t n, LexerState& lstate) noexcept;

TokenValue makeToken(std::string_view lexeme, LexerState& lstate) noexcept;

TokenType classifyWord(std::string_view word) noexcept;

} // Spark::Frontend
