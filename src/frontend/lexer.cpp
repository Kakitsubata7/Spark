#include "lexer.hpp"

#include <cstring>

#include "syntax.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

static Result<Token> streamErrorResult(const std::istream& stream, size_t line, size_t column) {
    return Result<Token>(Error(stream.eof() ? "unexpected end of input" : "stream error", line, column));
}

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

    // Read and check the first character
    if (!stream.get(c)) {
        return streamErrorResult(stream, line, column);
    }
    if (c != '_' && !std::isalpha(static_cast<unsigned char>(c))) {
        stream.unget();
        return Result<Token>(Error("invalid identifier/keyword", line, column));
    }
    s += c;
    lineAndColumnSuccessor(c, line, column);

    // Process the rest
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

Result<Token> Lexer::lexDecimalNumber(std::istream& stream, size_t& line, size_t& column) {
    std::string s;
    char c;

    // Read and check the first character
    if (!stream.get(c)) {
        return streamErrorResult(stream, line, column);
    }
    if (!std::isdigit(static_cast<unsigned char>(c)) && c != '.') {
        stream.unget();
        return Result<Token>(Error("invalid decimal number syntax", line, column));
    }
    s += c;
    lineAndColumnSuccessor(c, line, column);
    bool decimalPoint = c == '.'; // Initialize decimal point flag

    // Process the rest
    while (stream.get(c)) {
        // Check for decimal point
        if (!decimalPoint && c == '.') {
            decimalPoint = true;
            continue;
        }

        // _ is only allowed in the integer part
        if (!std::isdigit(c) && (!decimalPoint && c != '_')) {
            stream.unget();
            return Result<Token>(Error("invalid decimal number syntax", line, column));
        }

        s += c;
        lineAndColumnSuccessor(c, line, column);
    }

    // Reject trailing . or _
    if (!s.empty()) {
        char back = s.back();
        if (back == '.' || back == '_')
            return Result<Token>(Error("invalid decimal number syntax", line, column));
    }

    return Result(Token(TokenType::Number, s, line, column));
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

    // Check for comment and number literal syntaxes
    if (char next; stream.get(next)) {
        stream.unget();
        if (current == '/' && (next == '/' || next == '*')) {
            stream.unget();
            return lexComment(stream, line, column);
        }
        if (current == '0') {
            stream.unget();
            switch (next) {
                case 'b':
                    return lexBinaryNumber(stream, line, column);

                case 'o':
                    return lexOctalNumber(stream, line, column);

                case 'x':
                    return lexHexNumber(stream, line, column);

                default:
            }
        }
        if (std::isdigit(static_cast<unsigned char>(current))) {
            stream.unget();
            return lexDecimalNumber(stream, line, column);
        }
    }

    // Check for other token types
    stream.unget();
    if (std::isalpha(static_cast<unsigned char>(current)) || current == '_') {
        return lexWord(stream, line, column);
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
