#include "Lexer.hpp"

#include <cctype>

#include "LexException.hpp"

namespace Spark {

    /* ===== Tokenization ===== */

    std::unordered_set<std::string> Lexer::keywordSet = {
        "let"
    };

    std::unordered_set<std::string> Lexer::operatorSet = {
        "+", "+=", "++",
        "-", "-=", "--",
        "*", "*=",
        "/", "/=",
        "%", "%=",
        "=", "==", "=>",
        "!", "!=",
        "&", "&=", "&&",
        "|", "|=", "||",
        "^", "^=", "^^",
        "<", "<=", "<<", "<<=",
        ">", ">=", ">>", ">>=",
        ".",
        "?", "??", "\?\?=",
        ":", "::",
        ",",
        ";",
        "{",
        "}",
        "[",
        "]",
        "(",
        ")"
    };

    bool Lexer::isKeyword(const std::string& str) {
        return keywordSet.find(str) != keywordSet.end();
    }

    bool Lexer::isOperator(const std::string& str) {
        return operatorSet.find(str) != operatorSet.end();
    }

    bool Lexer::isOperator(char c) {
        return isOperator(std::string(1, c));
    }

    bool Lexer::isNumericalLiteral(const std::string& str) {
        std::istringstream iss(str);
        double num;
        iss >> num;
        return iss.eof() && !iss.fail();
    }

    bool Lexer::isIdentifier(const std::string& str) {
        // Empty string
        if (str.empty())
            return false;

        // First character has to be letters, '_' or '@'
        char firstChar = str[0];
        if (!std::isalpha(firstChar) && (firstChar != '_') && (firstChar != '@'))
            return false;

        // Rest characters have to be letters, digits or '_'
        for (size_t i = 1; i < str.length(); i++) {
            char c = str[i];
            if (!std::isalpha(c) && !std::isdigit(c) && (c != '_'))
                return false;
        }

        return true;
    }



    /* ===== Operations ===== */

    std::vector<Token> Lexer::lex(const std::string& str) {
        return Lexer(str).lex();
    }

    std::vector<Token> Lexer::lex() {
        std::vector<Token> tokens;

        std::string current;

        auto tokenizeCurrent = [&]() -> void {
            if (current.empty())
                return;

            if (Lexer::isKeyword(current))
                tokens.emplace_back(TokenType::Keyword, current);
            else if (Lexer::isOperator(current))
                tokens.emplace_back(TokenType::Operator, current);
            else if (Lexer::isNumericalLiteral(current))
                tokens.emplace_back(TokenType::NumericalLiteral, current);
            else if (Lexer::isIdentifier(current))
                tokens.emplace_back(TokenType::Identifier, current);
            else {
                std::stringstream ss;
                ss << "Invalid identifier: " << current;
                throw LexException(ss.str());
            }

            current.clear();
        };

        int c;
        while ((c = iss.get()) != EOF) {
            // Check for line commenting end
            if (state == State::LineComment) {
                // Check for LF
                if (c == '\n')
                    state = State::None;
                // Check for CR and CRLF
                else if (c == '\r') {
                    c = iss.peek();
                    if (c == EOF)
                        break;
                    if (c == '\n')
                        iss.ignore(1);

                    state = State::None;
                }

                continue;
            }
            // Check for group commenting end
            else if (state == State::GroupComment) {
                if (c == '*') {
                    c = iss.peek();
                    if (c == EOF)
                        break;

                    if (c == '/') {
                        state = State::None;
                        iss.ignore(1);
                    }
                }

                continue;
            }

            // Check for comment beginnings
            if (c == '/') {
                if ((c = iss.peek()) != EOF) {
                    // Check for line comment beginning
                    if (c == '/') {
                        state = State::LineComment;
                        iss.ignore(1);
                    }
                        // Check for group comment beginning
                    else if (c == '*') {
                        state = State::GroupComment;
                        iss.ignore(1);
                    }

                    continue;
                }
            }

            // TODO: Check for string beginnings

            // Check for numerical literal
            if (std::isdigit(c) || (c == '+') || (c == '-') || (c == '.')) {
                bool hasDigit = std::isdigit(c);
                bool hasDot = (c == '.');

                // Save current character and iss position
                int startC = c;
                std::streampos startPos = iss.tellg();

                // Build number
                std::string num = std::string(1, static_cast<char>(c));
                while ((c = iss.peek()) != EOF) {
                    if (c == '.') {
                        // Check for there's too many decimal points
                        if (hasDot)
                            throw LexException("Too many decimal points in a number");

                        hasDot = true;
                    } else {
                        // Check for end of the number
                        if (std::isspace(c) || isOperator(static_cast<char>(c)))
                            break;
                    }

                    if (std::isdigit(c))
                        hasDigit = true;

                    num += static_cast<char>(c);
                    iss.ignore(1);
                }

                // Tokenize number if it's valid (have at least one digit), otherwise resume
                if (hasDigit) {
                    tokens.emplace_back(TokenType::NumericalLiteral, num);
                    continue;
                }
                else {
                    c = startC;
                    iss.seekg(startPos);
                }
            }

            // Tokenize current if the current character is a white space character
            if (std::isspace(c)) {
                tokenizeCurrent();
                continue;
            }

            // Tokenize current if the current character is an operator
            if (Lexer::isOperator(static_cast<char>(c))) {
                // Tokenize current
                tokenizeCurrent();

                // Check if it's a sequence operator
                std::string op = std::string(1, static_cast<char>(c));
                while ((c = iss.peek()) != EOF) {
                    if (Lexer::isOperator(op + static_cast<char>(c))) {
                        op += static_cast<char>(c);
                        iss.get();
                    }
                    else
                        break;
                }

                // Tokenize operator
                tokens.emplace_back(TokenType::Operator, op);
                continue;
            }

            // Append character to the current token
            current += static_cast<char>(c);
        }

        // Throw lex exception if state is invalid
        switch (state) {
            case State::GroupComment:
                throw LexException("Unterminated group comment");

            case State::LineComment:
            case State::RawString:
            case State::FormattedString:
                throw LexException("Missing string terminating character");

            default:
                break;
        }

        // Tokenize current if it's not empty
        if (!current.empty())
            tokenizeCurrent();

        return tokens;
    }

} // Spark
