#include "lexer.hpp"

#include <cstring>

#include "syntax.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

static void lineAndColumnSuccessor(char c, size_t& line, size_t& column) {
    if (c == '\n') {
        ++line;
        column = 0;
    } else {
        ++column;
    }
}

std::pair<size_t, size_t> Lexer::countLineAndColumn(std::istream& stream) {
    const std::istream::iostate ogState = stream.rdstate();
    const std::streampos ogPos = stream.tellg();

    stream.clear();
    size_t line = 1, column = 0;
    char c;
    while (stream.get(c)) {
        lineAndColumnSuccessor(c, line, column);
    }

    stream.clear();
    if (ogPos != std::streampos(-1)) {
        stream.seekg(ogPos);
    }
    stream.clear(ogState);

    return {line, column};
}

Result<Token> Lexer::lexWord(std::istream& stream, size_t& line, size_t& column) {
    std::string s;
    char c;

    if (!stream.get(c)) {
        return Result<Token>(Error(stream.eof() ? "unexpected end of input" : "stream error", line, column));
    }

    if (c != '_' && !std::isalpha(static_cast<unsigned char>(c))) {
        stream.unget();
        return Result<Token>(Error("invalid identifier/keyword", line, column));
    }
    s += c;
    lineAndColumnSuccessor(c, line, column);
    while (stream.get(c)) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (c == '_' || std::isalpha(uc) || std::isdigit(uc)) {
            s += c;
            lineAndColumnSuccessor(c, line, column);
        } else {
            stream.unget();
            break;
        }
    }

    return Result(Token(Syntax::isKeyword(s) ? TokenType::Keyword : TokenType::Identifier, s, line, column));
}


Result<Token> Lexer::lex(std::istream& stream, size_t& line, size_t& column) {
    // Skip space characters
    char current;
    while (true) {
        if (!stream.get(current)) {
            if (stream.eof()) {
                return Result(Token(TokenType::EndOfFile, "", line, column));
            }
            return Result<Token>(Error("stream error", line, column)); // TODO
        }
        lineAndColumnSuccessor(current, line, column);

        if (!std::isspace(static_cast<unsigned char>(current))) {
            break;
        }
    }

    // Check for comment syntax
    if (char next; stream.get(next)) {
        stream.unget();
        if (current == '/' && (next == '/' || next == '*')) {
            stream.unget();
            return lexComment(stream, line, column);
        }
    }

    // Check for other token types
    stream.unget();
    if (std::isalpha(static_cast<unsigned char>(current)) || current == '_') {
        return lexWord(stream, line, column);
    }
    if (std::isdigit(static_cast<unsigned char>(current))) {
        return lexNumber(stream, line, column);
    }
    if (current == '"' || current == '\'') {
        return lexString(stream, line, column);
    }
    if (std::ispunct(static_cast<unsigned char>(current))) {
        return lexPunctuator(stream, line, column);
    }

    return Result<Token>(Error("unexpected character", line, column));
}

Result<Token> Lexer::lex() const {
    auto [line, column] = countLineAndColumn(_stream);
    return lex(_stream, line, column);
}

Result<std::vector<Token>> Lexer::lexAll() const {
    std::vector<Token> tokens;
    auto [line, column] = countLineAndColumn(_stream);
    while (true) {
        Result<Token> result = lex(_stream, line, column);
        if (Token token; result.tryGetValue(token)) {
            tokens.push_back(token);
            if (token.type == TokenType::EndOfFile) {
                break;
            }
        } else {
            return Result<std::vector<Token>>(result.error());
        }
    }
    return Result(tokens);
}
