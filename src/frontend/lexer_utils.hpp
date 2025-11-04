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
TokenValue makeToken(std::string_view lexeme, size_t startColumn, LexerState& lstate) noexcept;

TokenType classifyWord(std::string_view word) noexcept;

void clearTokenBuffer(LexerState& lstate) noexcept;

void appendTokenBuffer(LexerState& lstate, std::string_view sv) noexcept;
void appendTokenBuffer(LexerState& lstate, char c) noexcept;

void raiseError(LexerState& lstate, std::string_view message) noexcept;

} // Spark::Frontend
