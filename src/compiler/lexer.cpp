#include "lexer.hpp"

#include <unordered_set>

namespace spark {

/* ===== Syntax ===== */

namespace {

const std::unordered_set<std::string_view> keywords {
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

const Trie symbols {
    ",", ";",
    ".", "?", "!", ":", "->",
    "+", "-", "*", "/", "%", "&", "|", "^", "~",
    "++", "--",
    "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
    "==", "!=", "<", ">", "<=", ">=",
    "(", ")", "[", "]", "{", "}",
    "@"
};

}

bool Lexer::isKeyword(std::string_view sv) {
    return keywords.find(sv) != keywords.end();
}

Lexer::Match Lexer::matchSymbol(std::string_view sv) {
    return symbols.match(sv);
}

bool Lexer::isIdentifier(std::string_view sv) {
    // Empty string is not an identifier
    if (sv.empty()) {
        return false;
    }

    // The first character must be an English alphabet or an underscore
    if (!std::isalpha(sv[0]) && sv[0] != '_') {
        return false;
    }

    // The rest must be an English alphabet, a digit, or an underscore
    for (size_t i = 1; i < sv.length(); i++) {
        if (!std::isalpha(sv[1]) && !std::isdigit(sv[i]) && sv[i] != '_') {
            return false;
        }
    }

    return true;
}



/* ===== Operations ===== */

Result<std::vector<Token>> Lexer::lex(std::istream& is) {
    Result<std::vector<Token>> result;
    std::vector<Token> tokens;

    std::string current;

    char c;
    while (is.get(c)) {
        // TODO
    }

    // Set lexing result
    result.value = result.hasError() ? std::nullopt : std::make_optional<std::vector<Token>>(tokens);

    return result;
}

} // spark
