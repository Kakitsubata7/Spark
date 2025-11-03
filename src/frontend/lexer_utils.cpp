#include "lexer_utils.hpp"

#include <unordered_map>

namespace Spark::FrontEnd {

namespace {
    const std::unordered_map<std::string_view, yytoken_kind_t> keywordTokenMap {
        { "alias", SPK_ALIAS }, { "as", SPK_AS },
        { "break", SPK_BREAK },
        { "case", SPK_CASE }, { "catch", SPK_CATCH }, { "class", SPK_CLASS }, { "const", SPK_CONST },
            { "constructor", SPK_CONSTRUCTOR }, { "continue", SPK_CONTINUE }, { "cref", SPK_CREF },
        { "destructor", SPK_DESTRUCTOR }, { "do", SPK_DO },
        { "else", SPK_ELSE }, { "end", SPK_END }, { "enum", SPK_ENUM }, { "export", SPK_EXPORT },
            { "extension", SPK_EXTENSION },
        { "false", SPK_FALSE }, { "fn", SPK_FN }, { "for", SPK_FOR }, { "from", SPK_FROM },
        { "global", SPK_GLOBAL },
        { "if", SPK_IF }, { "import", SPK_IMPORT }, { "in", SPK_IN }, { "is", SPK_IS },
        { "let", SPK_LET },
        { "match", SPK_MATCH }, { "module", SPK_MODULE },
        { "nil", SPK_NIL },
        { "operator", SPK_OPERATOR },
        { "ref", SPK_REF }, { "return", SPK_RETURN},
        { "self", SPK_SELF }, { "struct", SPK_STRUCT }, { "super", SPK_SUPER },
        { "then", SPK_THEN }, { "throw", SPK_THROW }, { "trait", SPK_TRAIT }, { "true", SPK_TRUE },
            { "try", SPK_TRY },{ "typeof", SPK_TYPEOF },
        { "undefined", SPK_UNDEFINED },
        { "while", SPK_WHILE },
        { "yield", SPK_YIELD }
    };
}

TokenType makeWord(std::string_view sv, LexerState& state) noexcept {
    yylval = TokenValue(std::string(sv), state.line, state.column);
    state.line += sv.size();
    if (keywordTokenMap.find(sv) != keywordTokenMap.end()) {
        return keywordTokenMap.at(sv);
    }
    if (sv == "_") {
        return SPK_DISCARD;
    }
    return SPK_IDENTIFIER;
}

TokenType makeInteger(std::string_view sv, LexerState& state) noexcept {
    yylval = TokenValue(std::string(sv), state.line, state.column);
    state.line += sv.size();
    return SPK_INTEGER;
}

TokenType makeReal(std::string_view sv, LexerState& state) noexcept {
    yylval = TokenValue(std::string(sv), state.line, state.column);
    state.line += sv.size();
    return SPK_REAL;
}

TokenType makeLineComment(std::string_view sv, LexerState& state) noexcept {
    yylval = TokenValue(std::string(sv), state.line, state.column);
    state.line += sv.size();
    return SPK_LINE_COMMENT;
}

void whenNewline(LexerState& state) noexcept {
    state.line++;
    state.column = 1;
}

void consumeCharacters(LexerState& state, size_t n) noexcept {
    state.column += n;
}

} // Spark::FrontEnd


