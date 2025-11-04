#include "lexer_utils.hpp"

#include <unordered_map>

namespace Spark::FrontEnd {

namespace {
    const std::unordered_map<std::string_view, TokenType> keywordTokenMap {
        { "alias", TokenType::Alias }, { "as", TokenType::As },
        { "break", TokenType::Break },
        { "case", TokenType::Case }, { "catch", TokenType::Catch }, { "class", TokenType::Class },
            { "const", TokenType::Const }, { "constructor", TokenType::Constructor },
            { "continue", TokenType::Continue }, { "cref", TokenType::Cref },
        { "destructor", TokenType::Destructor }, { "do", TokenType::Do },
        { "else", TokenType::Else }, { "end", TokenType::End }, { "enum", TokenType::Enum },
            { "export", TokenType::Export }, { "extension", TokenType::Extension },
        { "false", TokenType::False }, { "fn", TokenType::Fn }, { "for", TokenType::For },
            { "from", TokenType::From }, { "global", TokenType::Global },
        { "if", TokenType::If }, { "import", TokenType::Import }, { "in", TokenType::In },
            { "is", TokenType::Is },
        { "let", TokenType::Let },
        { "match", TokenType::Match }, { "module", TokenType::Module },
        { "nil", TokenType::Nil },
        { "operator", TokenType::Operator },
        { "ref", TokenType::Ref }, { "return", TokenType::Return },
        { "self", TokenType::Self }, { "struct", TokenType::Struct }, { "super", TokenType::Super },
        { "then", TokenType::Then }, { "throw", TokenType::Throw }, { "trait", TokenType::Trait },
            { "true", TokenType::True }, { "try", TokenType::Try }, { "typeof", TokenType::Typeof },
        { "undefined", TokenType::Undefined },
        { "while", TokenType::While },
        { "yield", TokenType::Yield }
    };
}

void handleNewline(LexerState& lstate) noexcept {
    ++lstate.line;
    lstate.column = 1;
}

void consumeCharacters(LexerState& lstate, size_t n) noexcept {
    lstate.column += n;
}

TokenValue makeToken(std::string_view lexeme, LexerState& lstate) noexcept {
    lstate.line += lexeme.size();
    return TokenValue(std::string(lexeme), lstate.line, lstate.column);
}

TokenType classifyWord(std::string_view word) noexcept {
    if (keywordTokenMap.find(word) != keywordTokenMap.end()) {
        return keywordTokenMap.at(word);
    }
    return word == "_" ? TokenType::Discard : TokenType::Identifier;
}

} // Spark::FrontEnd


