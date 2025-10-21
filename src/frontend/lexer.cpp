#include "lexer.hpp"

#include <cstring>

#include "syntax.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

template <typename T>
static Result<T> streamErrorResult(size_t line, size_t column) {
    return Result<T>(Error("stream error", line, column));
}

template <typename T>
static Result<T> streamErrorResult(const std::istream& stream, size_t line, size_t column, const std::string& eofMsg) {
    if (stream.eof()) {
        return Result<T>(Error(eofMsg, line, column));
    }
    return streamErrorResult<T>(line, column);
}

static void lineAndColumnSuccessor(char c, size_t& line, size_t& column) {
    if (c == '\n') {
        ++line;
        column = 0;
    } else {
        ++column;
    }
}

Result<Token> Lexer::lexWord(std::istream& stream, size_t& line, size_t& column) {
    size_t startColumn = column;
    std::string s;
    char c;

    // Read and check the first character
    if (!stream.get(c)) {
        return streamErrorResult<Token>(stream, line, column, "invalid identifier/keyword");
    }
    if (c != '_' && !std::isalpha(static_cast<unsigned char>(c))) {
        stream.unget();
        return Result<Token>(Error("invalid identifier/keyword", line, column));
    }
    s += c;
    ++column;

    // Process the rest
    while (stream.get(c)) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (c == '_' || std::isalpha(uc) || std::isdigit(uc)) {
            s += c;
            ++column;
        } else {
            stream.unget();
            break;
        }
    }

    return Result(Token(Syntax::isKeyword(s) ? TokenType::Keyword : TokenType::Identifier, s, line, startColumn));
}

Result<Token> Lexer::lexDecimalNumber(std::istream& stream, size_t& line, size_t& column) {
    std::string s;
    char c;

    // Read and check the first character
    if (!stream.get(c)) {
        return streamErrorResult<Token>(stream, line, column, "invalid number syntax");
    }
    if (!std::isdigit(static_cast<unsigned char>(c)) && c != '.') {
        stream.unget();
        return Result<Token>(Error("invalid number syntax", line, column));
    }
    s += c;
    ++column;
    bool decimalPoint = c == '.'; // Initialize decimal point flag

    // Process the rest
    while (stream.get(c)) {
        unsigned char uc = static_cast<unsigned char>(c);

        // Digit
        if (std::isdigit(uc)) {
            s += c;
            ++column;
            continue;
        }

        // Check for decimal point
        if (c == '.') {
            if (decimalPoint) {
                return Result<Token>(Error("invalid number syntax", line, column));
            }
            decimalPoint = true;
            s += c;
            ++column;
            continue;
        }

        // _ is only allowed in the integer part
        if (!decimalPoint && c == '_') {
            s += c;
            ++column;
            continue;
        }

        // Separator characters, terminate
        if (Syntax::isPunctuator(c)) {
            stream.unget();
            break;
        }

        // Other characters, reject
        stream.unget();
        return Result<Token>(Error("invalid number syntax", line, column));
    }
    if (stream.fail() && !stream.eof()) {
        return streamErrorResult<Token>(line, column);
    }

    // Reject trailing . or _
    if (!s.empty()) {
        char back = s.back();
        if (back == '.' || back == '_') {
            return Result<Token>(Error("invalid number syntax", line, column));
        }
    }

    return Result(Token(TokenType::Number, s, line, column));
}

Result<Token> Lexer::lexBinaryNumber(std::istream& stream, size_t& line, size_t& column) {
    size_t startColumn = column;
    std::string s;

    // Make sure starting with 0b
    if (streamStartWith(stream, "0b")) {
        s += "0b";
    } else if (streamStartWith(stream, "0B")) {
        s += "0B";
    } else {
        return streamErrorResult<Token>(stream, line, column, "invalid binary number syntax");
    }
    stream.ignore(2);
    column += 2;

    // Check digits
    char c;
    while (stream.get(c)) {
        // 0, 1 and _
        if (c == '0' || c == '1' || c == '_') {
            s += c;
            ++column;
            continue;
        }

        // Separator characters, terminate
        if (Syntax::isPunctuator(c)) {
            stream.unget();
            break;
        }

        // Other characters, reject
        stream.unget();
        return Result<Token>(Error("invalid binary number syntax", line, column));
    }
    if (stream.fail() && !stream.eof()) {
        return streamErrorResult<Token>(line, column);
    }

    // Reject empty binary number
    if (s.size() == 2) {
        return Result<Token>(Error("invalid binary number syntax", line, column));
    }

    // Reject leading and trailing _
    if (!s.empty()) {
        if (s[2] == '_' || s.back() == '_') {
            return Result<Token>(Error("invalid binary number syntax", line, column));
        }
    }

    return Result(Token(TokenType::Number, s, line, startColumn));
}

static constexpr bool isOctalDigit(char c) noexcept {
    unsigned char uc = static_cast<unsigned char>(c);
    return uc >= '0' && uc <= '7';
}

Result<Token> Lexer::lexOctalNumber(std::istream& stream, size_t& line, size_t& column) {
    std::string s;

    // Make sure starting with 0o
    if (streamStartWith(stream, "0o")) {
        s += "0o";
    } else if (streamStartWith(stream, "0O")) {
        s += "0O";
    } else {
        return Result<Token>(Error("invalid octal number syntax", line, column));
    }
    stream.ignore(2);
    column += 2;

    // Check digits
    char c;
    while (stream.get(c)) {
        // Octal digits and _
        if (isOctalDigit(c) || c == '_') {
            s += c;
            ++column;
            continue;
        }

        // Separator characters, terminate
        if (Syntax::isPunctuator(c)) {
            stream.unget();
            break;
        }

        // Other characters, reject
        stream.unget();
        return Result<Token>(Error("invalid octal number syntax", line, column));
    }
    if (stream.fail() && !stream.eof()) {
        return streamErrorResult<Token>(line, column);
    }

    // Reject empty octal number
    if (s.size() == 2) {
        return Result<Token>(Error("invalid octal number syntax", line, column));
    }

    // Reject leading and trailing _
    if (!s.empty()) {
        if (s[2] == '_' || s.back() == '_') {
            return Result<Token>(Error("invalid octal number syntax", line, column));
        }
    }

    return Result(Token(TokenType::Number, s, line, column));
}

static constexpr bool isHexDigit(char c) noexcept {
    unsigned char uc = static_cast<unsigned char>(c);
    return (uc >= '0' && uc <= '9') ||
           (uc >= 'a' && uc <= 'f') ||
           (uc >= 'A' && uc <= 'F');
}

Result<Token> Lexer::lexHexNumber(std::istream& stream, size_t& line, size_t& column) {
    std::string s;

    // Make sure starting with 0x
    if (streamStartWith(stream, "0x")) {
        s += "0x";
    } else if (streamStartWith(stream, "0X")) {
        s += "0X";
    } else {
        return Result<Token>(Error("invalid hexadecimal number syntax", line, column));
    }
    stream.ignore(2);
    column += 2;

    // Check digits
    char c;
    while (stream.get(c)) {
        // Hexadecimal digits and _
        if (isHexDigit(c) || c == '_') {
            s += c;
            ++column;
            continue;
        }

        // Separator characters, terminate
        if (Syntax::isPunctuator(c)) {
            stream.unget();
            break;
        }

        // Other characters, reject
        stream.unget();
        return Result<Token>(Error("invalid hexadecimal number syntax", line, column));
    }
    if (stream.fail() && !stream.eof()) {
        return streamErrorResult<Token>(line, column);
    }

    // Reject empty hexadecimal number
    if (s.size() == 2) {
        return Result<Token>(Error("invalid hexadecimal number syntax", line, column));
    }

    // Reject leading and trailing _
    if (!s.empty()) {
        if (s[2] == '_' || s.back() == '_') {
            return Result<Token>(Error("invalid hexadecimal number syntax", line, column));
        }
    }

    return Result(Token(TokenType::Number, s, line, column));
}

constexpr bool isValidStringChar(char c, char quote) noexcept {
    unsigned char uc = static_cast<unsigned char>(c);
    return (uc >= 0x20 || uc == '\t' || uc >= 0x80) && uc != '\\' && uc != static_cast<unsigned char>(quote);
}

Result<Token> Lexer::lexString(std::istream& stream, size_t& line, size_t& column) {
    char c;

    // Make sure starting with " or '
    if (!stream.get(c)) {
        return streamErrorResult<Token>(stream, line, column, "invalid string syntax");
    }
    if (c != '"' && c != '\'') {
        stream.unget();
        return Result<Token>(Error("invalid string syntax", line, column));
    }
    ++column;
    char quote = c;

    // Content
    std::string s;
    bool terminated = false;
    while (stream.get(c)) {
        // Check for string end
        if (c == quote) {
            ++column;
            terminated = true;
            break;
        }

        // Check for escape sequence
        if (c == '\\') {
            stream.unget();
            Result<std::string> esResult = lexEscapeSequence(stream, line, column);
            if (std::string escape; esResult.tryGetValue(escape)) {
                s += escape;
                continue;
            }
            return Result<Token>(esResult.error());
        }

        // Reject invalid characters
        if (!isValidStringChar(c, quote)) {
            stream.unget();
            return Result<Token>(Error("invalid character in string", line, column));
        }

        s += c;
        ++column;
    }
    if (stream.fail() && !stream.eof()) {
        return streamErrorResult<Token>(line, column);
    }

    // Make sure string is terminated
    if (!terminated) {
        return Result<Token>(Error("unterminated string", line, column));
    }

    return Result(Token(TokenType::String, s, line, column));
}

Result<std::string> Lexer::lexEscapeSequence(std::istream& stream, size_t& line, size_t& column) {
    char c;

    // Make sure starting with backslash
    if (!stream.get(c)) {
        return streamErrorResult<std::string>(stream, line, column, "invalid escape sequence syntax");
    }
    if (c != '\\') {
        stream.unget();
        return Result<std::string>(Error("invalid escape sequence syntax", line, column));
    }
    ++column;

    // Process escape sequences
    if (!stream.get(c)) {
        return streamErrorResult<std::string>(stream, line, column, "invalid escape sequence syntax");
    }
    std::string s;
    switch (c) {
        case '\'':
            s = "'";
            break;

        case '"':
            s = "\"";
            break;

        case '?':
            s = "?";
            break;

        case '\\':
            s = "\\";
            break;

        case 'a':
            s = "\a";
            break;

        case 'b':
            s = "\b";
            break;

        case 'f':
            s = "\f";
            break;

        case 'n':
            s = "\n";
            break;

        case 'r':
            s = "\r";
            break;

        case 't':
            s = "\t";
            break;

        case 'v':
            s = "\v";
            break;

        case '0':
            s = std::string("\0", 1);
            break;

        case 'x':
            // TODO
            break;

        case 'u':
            // TODO
            break;

        default:
            return Result<std::string>(Error("invalid escape sequence syntax", line, column));
    }
    ++column;
    return Result(s);
}

Result<Token> Lexer::lexPunctuator(std::istream& stream, size_t& line, size_t& column) {
    char c;

    // Lex directly if it's a non-composite delimiter
    if (!stream.get(c)) {
        return streamErrorResult<Token>(stream, line, column, "invalid punctuator");
    }
    if (Syntax::isNonCompositeDelimiter(c)) {
        ++column;
        return Result(Token(TokenType::Delimiter, std::string(1, c), line, column));
    }
    ++column;

    // Process the rest
    std::string s(1, c);
    while (stream.get(c)) {
        if (!Syntax::isPunctuator(c)) {
            stream.unget();
            break;
        }
        s += c;
        ++column;
    }
    if (stream.fail() && !stream.eof()) {
        return streamErrorResult<Token>(line, column);
    }

    if (Syntax::isOperator(s)) {
        return Result(Token(TokenType::Operator, s, line, column));
    }
    if (Syntax::isDelimiter(s)) {
        return Result(Token(TokenType::Delimiter, s, line, column));
    }
    return Result<Token>(Error("unknown punctuator", line, column));
}

Result<Token> Lexer::lexLineComment(std::istream& stream, size_t& line, size_t& column) {
    // Make sure starting with //
    if (!streamStartWith(stream, "//")) {
        return Result<Token>(Error("invalid line comment syntax", line, column));
    }
    stream.ignore(2);
    column += 2;

    // Content
    std::string s;
    char c;
    while (stream.get(c)) {
        // Stop at newline
        if (c == '\n') {
            lineAndColumnSuccessor(c, line, column);
            break;
        }

        s += c;
        ++column;
    }
    if (stream.fail() && !stream.eof()) {
        return streamErrorResult<Token>(line, column);
    }

    return Result(Token(TokenType::Comment, s, line, column));
}

Result<Token> Lexer::lexGroupComment(std::istream& stream, size_t& line, size_t& column) {
    // Make sure starting with /*
    if (!streamStartWith(stream, "/*")) {
        return Result<Token>(Error("invalid group comment syntax", line, column));
    }
    stream.ignore(2);
    column += 2;

    // Content
    std::string s;
    char c, prev = '\0';
    bool terminated = false;
    while (stream.get(c)) {
        // Check for group comment end
        if (prev == '*' && c == '/') {
            s.pop_back();
            terminated = true;
            break;
        }

        s += c;
        lineAndColumnSuccessor(c, line, column);
        prev = c;
    }
    if (stream.fail() && !stream.eof()) {
        return streamErrorResult<Token>(line, column);
    }
    if (!terminated) {
        return Result<Token>(Error("unterminated group comment", line, column));
    }

    return Result(Token(TokenType::Comment, s, line, column));
}

Result<Token> Lexer::lex(std::istream& stream, size_t& line, size_t& column) {
    // Skip space characters
    char current;
    while (true) {
        if (!stream.get(current)) {
            if (stream.eof()) {
                return Result(Token(TokenType::EndOfFile, "", line, column));
            }
            return streamErrorResult<Token>(line, column);
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
            if (next == '/') {
                return lexLineComment(stream, line, column);
            }
            return lexGroupComment(stream, line, column);
        }
        if (current == '0') {
            stream.unget();
            switch (next) {
                case 'b': case 'B':
                    return lexBinaryNumber(stream, line, column);

                case 'o': case 'O':
                    return lexOctalNumber(stream, line, column);

                case 'x': case 'X':
                    return lexHexNumber(stream, line, column);

                default:
                    break;
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
    if (Syntax::isPunctuator(current)) {
        return lexPunctuator(stream, line, column);
    }

    return Result<Token>(Error("unexpected character", line, column));
}

Result<Token> Lexer::lex() {
    auto [line, column] = countLineAndColumn(_stream);
    return lex(_stream, line, column);
}

Result<std::vector<Token>> Lexer::lexAll() {
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

std::pair<size_t, size_t> Lexer::countLineAndColumn(std::istream& stream) {
    const std::istream::iostate ogState = stream.rdstate();
    const std::streampos ogPos = stream.tellg();

    stream.clear();
    size_t line = 1, column = 1;
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

bool Lexer::streamStartWith(std::istream& stream, std::string_view sv) {
    if (!stream.good()) {
        return false;
    }
    if (sv.empty()) {
        return true;
    }

    // Save stream position
    std::streampos ogPos = stream.tellg();

    std::string buffer(sv.size(), '\0');
    stream.read(&buffer[0], static_cast<std::streamsize>(sv.size()));
    std::streamsize gcount = stream.gcount();

    // Restore stream position
    stream.clear();
    stream.seekg(ogPos);

    return gcount == static_cast<std::streamsize>(sv.size()) && buffer == sv;
}

bool Lexer::streamStartWithIgnoreCase(std::istream& stream, std::string_view sv) {
    if (!stream.good()) {
        return false;
    }
    if (sv.empty()) {
        return true;
    }

    // Save stream position
    std::streampos ogPos = stream.tellg();

    std::string buffer(sv.size(), '\0');
    stream.read(&buffer[0], static_cast<std::streamsize>(sv.size()));
    std::streamsize gcount = stream.gcount();

    // Restore stream position
    stream.clear();
    stream.seekg(ogPos);

    // Read counts don't match
    if (gcount != static_cast<std::streamsize>(sv.size())) {
        return false;
    }

    // Compare case-insensitively
    for (size_t i = 0; i < sv.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(buffer[i])) != std::tolower(static_cast<unsigned char>(sv[i]))) {
            return false;
        }
    }

    return true;
}
