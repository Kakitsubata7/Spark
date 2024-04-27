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
            else
                tokens.emplace_back(TokenType::Identifier, current);

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

            // Tokenize current if the current character is a white space character
            if (isspace(c)) {
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
