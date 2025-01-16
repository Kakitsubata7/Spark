#pragma once

#include <array>

namespace spark::syntax {

static std::array<const char*, 12> keywords = {
    "do", "end",
    "if", "else",
    "for", "while",
    "let", "const",
    "fn",
    "not", "and", "or"
};

static std::array<const char*, 19> operators = {
    ",", ";",
    ".", ":",
    "=",
    "+", "-", "*", "/", "%", "&", "|", "~", "==", "!=",
    "(", ")", "[", "]",
};

} // spark::syntax
