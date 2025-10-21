#pragma once

#include <string_view>

namespace Spark::FrontEnd::Syntax {

bool isKeyword(std::string_view s) noexcept;

bool isOperator(std::string_view s) noexcept;

bool isDelimiter(std::string_view s) noexcept;

bool isNonCompositeDelimiter(char c) noexcept;

bool isPunctuator(char c) noexcept;

} // Spark::FrontEnd::Syntax
