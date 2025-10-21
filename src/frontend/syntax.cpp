#include "syntax.hpp"

#include <array>
#include <unordered_set>

namespace Spark::FrontEnd::Syntax {

namespace {
    const std::unordered_set<std::string_view> keywords {
        "alias", "as",
        "break",
        "case", "catch", "class", "const", "constructor", "continue", "cref",
        "destructor", "do",
        "else", "end", "enum", "export", "extension",
        "false", "fn", "for", "from",
        "global",
        "if", "import", "in", "is",
        "let",
        "match", "module",
        "nil",
        "operator",
        "ref", "return",
        "self", "struct", "super",
        "then", "throw", "trait", "true", "try", "typeof",
        "undefined",
        "while",
        "yield"
    };

    const std::unordered_set<std::string_view> operators {
        "+", "-", "*", "/", "%",
        "~", "&", "|", "^", "<<", ">>",
        "!", "&&", "||",
        "==", "!=", ">", "<", ">=", "<=",
        "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=",
        "."
    };

    const std::unordered_set<std::string_view> delimiters {
        ",", ":", "->", "=>"
    };

    constexpr std::array<bool, 256> makeNonCompositeDelimiterTable() {
        std::array<bool, 256> table{};
        for (char c : { ';', '(', ')', '[', ']', '{', '}', '<', '>' }) {
            table[static_cast<unsigned char>(c)] = true;
        }
        return table;
    }

    constexpr std::array<bool, 256> nonCompositeDelimiterTable = makeNonCompositeDelimiterTable();

    constexpr std::array<bool, 256> makeSeparatorTable() {
        std::array<bool, 256> table{};
        for (char c : { '+', '-', '*', '/', '%', '~', '&', '|', '^', '<', '>', '!', '=', '.' }) {
            table[static_cast<unsigned char>(c)] = true;
        }
        for (char c : { ',', ':', ';', '(', ')', '[', ']', '{', '}', '<', '>', '-', '=' }) {
            table[static_cast<unsigned char>(c)] = true;
        }
        return table;
    }

    constexpr std::array<bool, 256> separatorTable = makeSeparatorTable();
}

bool isKeyword(std::string_view s) noexcept {
    return keywords.find(s) != keywords.end();
}

bool isOperator(std::string_view s) noexcept {
    return operators.find(s) != operators.end();
}

bool isDelimiter(std::string_view s) noexcept {
    return (s.size() == 1 && nonCompositeDelimiterTable[static_cast<unsigned char>(s.front())]) ||
           delimiters.find(s) != delimiters.end();
}

bool isNonCompositeDelimiter(char c) noexcept {
    return nonCompositeDelimiterTable[static_cast<unsigned char>(c)];
}

bool isPunctuator(char c) noexcept {
    return separatorTable[static_cast<unsigned char>(c)];
}

} // Spark::FrontEnd::Syntax
