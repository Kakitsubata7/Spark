#include "lexer.hpp"

#include <algorithm>
#include <array>
#include <istream>

namespace spark {

/* ===== Syntax ===== */

namespace {

bool isSyntaxInitialized = false;

std::array<std::string_view, 28> keywords = {
    "do", "end",
    "if", "else",
    "match", "case",
    "for", "while", "continue", "break",
    "let", "const",
    "fn",
    "not", "and", "or",
    "is", "in",
    "yield",
    "undefined", "nil", "true", "false",
    "import",
    "class", "struct", "enum", "alias"
};

std::array<std::string_view, 38> symbols = {
    ",", ";",
    ".", "?", "!", ":", "->"
    "+", "-", "*", "/", "%", "&", "|", "^", "~",
    "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
    "==", "!=", "<", ">", "<=", ">=",
    "(", ")", "[", "]", "{", "}",
    "@"
};

}

void Lexer::initializeSyntax() {
    if (isSyntaxInitialized) { return; }
    isSyntaxInitialized = true;

    std::sort(keywords.begin(), keywords.end());
}



/* ===== Operations ===== */

namespace {

[[nodiscard]]
bool isKeyword(const std::string& s) {
    return std::binary_search(keywords.cbegin(), keywords.cend(), s);
}

enum class Match {
    Possible,
    Equal,
    Impossible
};

[[nodiscard]]
Match matchKeyword(const std::string& s) {
    const std::string_view* it = std::lower_bound(keywords.cbegin(), keywords.cend(), s);
    if (it != keywords.cend() && it->compare(0, s.length(), s.c_str()) == 0) {
        if (s.length() < it->length()) {
            return Match::Possible;
        }
        return Match::Equal;
    }
    return Match::Impossible;
}

[[nodiscard]]
Token tokenizeKeyword(const std::string& s) {
    if (s == "true" || s == "false") {
        return {TokenType::Boolean, s};
    }
    if (s == "nil") {
        return {TokenType::Nil, s};
    }
    return {TokenType::Keyword, s};
}

[[nodiscard]]
bool isIdentifier(const std::string& s) {
    // Empty string is not identifier
    if (s.empty()) {
        return false;
    }

    // First character must be letters or underscore
    if (!std::isalpha(s[0]) && s[0] != '_') {
        return false;
    }

    // The rest characters must be letters, digits, or underscore
    for (size_t i = 1; i < s.size(); i++) {
        if (!std::isalnum(s[i]) && s[i] != '_') {
            return false;
        }
    }

    return true;
}

[[nodiscard]]
Token tokenizeIdentifier(const std::string& s) {
    if (s == "_") {
        return {TokenType::Ignore, "_"};
    }
    return {TokenType::Identifier, s};
}

[[nodiscard]]
bool isDelimiter(char c) {
    if (std::isspace(c)) {
        return true;
    }
    if () {
        return true;
    }
    return false;
}

}

Result<std::vector<Token>> Lexer::lex(std::istream& is) {
    Result<std::vector<Token>> result;
    std::vector<Token> tokens;

    std::string current;

    // General matching
    while (true) {

    }

    // Set lexing result
    result.value = result.hasError() ? std::nullopt : std::make_optional<std::vector<Token>>(tokens);

    return result;
}

} // spark
