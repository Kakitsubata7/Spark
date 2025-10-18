#pragma once

#include <string>
#include <unordered_set>

namespace Spark::FrontEnd::Syntax {

const std::unordered_set<std::string> keywords {
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

const std::unordered_set<std::string> operators {
    "+", "-", "*", "/", "%",
    "~", "&", "|", "^", "<<", ">>",
    "!", "&&", "||",
    "==", "!=", ">", "<", ">=", "<=",
    "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="
};

const std::unordered_set<std::string> delimiters {
    ",", ".", ":", ";",
    "(", ")", "[", "]", "{", "}", "<", ">",
    "->",
    "=>"
};

inline bool isKeyword(const std::string& s) {
    return keywords.find(s) != keywords.end();
}

inline bool isOperator(const std::string& s) {
    return operators.find(s) != operators.end();
}

inline bool isDelimiter(const std::string& s) {
    return delimiters.find(s) != delimiters.end();
}

} // Spark::FrontEnd::Syntax
