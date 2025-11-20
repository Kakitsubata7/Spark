#pragma once

#include <string_view>

#include "lexer_utils.hpp"
#include "token_type.hpp"

namespace Spark::Compiler {

TokenType classifyWord(std::string_view word) noexcept;

} // Spark::Compiler
