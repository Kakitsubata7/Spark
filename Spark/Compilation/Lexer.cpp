#include "Lexer.hpp"

#include <cctype>

#include <iostream>

namespace Spark {

    /* ===== Operations ===== */

    static bool isKeyword(const std::string& str) {

        static const std::unordered_set<std::string> keywordSet = {
            "function",
            "if", "else",
            "for", "while", "do",
            "switch", "case"
        };

        return keywordSet.find(str) != keywordSet.end();
    }

    static bool isOperator(const std::string& str) {

        static const std::unordered_set<std::string> operatorSet = {
            ".", "?", ":",
            "+", "-", "*", "/", "%",
            "=", "+=", "-=", "*=", "/=", "%=",
            "!", "&", "|", "^", "&&", "||", "<<", ">>",
            "==", "!=", "<", ">", "<=", ">=",
        };

        return operatorSet.find(str) != operatorSet.end();
    }

    static bool isSeparator(char c) {

        static const std::unordered_set<char> separatorSet = {
            ',', ';', '(', ')', '[', ']', '{', '}',
        };

        return separatorSet.find(c) != separatorSet.end();
    }

    static std::vector<std::string> lexHelper(const char* p) {

        // Make sure the pointer is not null
        if (p == nullptr)
            throw std::runtime_error("Null pointer.");

        std::vector<std::string> tokens;
        std::string current;

        while (*p != '\0') {

            char c = *p;

            // Terminate when encountering a null character
            if (c == '\0')
                break;

            // Ignore space characters
            if (isspace(c)) {
                p++;
                continue;
            }

            // Check for comment beginnings
            if (c == '/') {
                char next = *(p + 1);

                // Check for line comment beginning
                if (next == '/') {

                    // Move the pointer after the checked characters
                    p += 2;

                    // Skip until encountering a null character or a newline
                    while (true) {

                        c = *p;

                        // Terminate when encountering a null character
                        if (c == '\0')
                            break; // Terminate this while loop, so it will go to the next iteration of the main loop

                        // Stop commenting when encountering any newline representation
                        if (c == '\n') {
                            // LF
                            p++; // Skip this '\n' character
                            break;
                        }
                        if (c == '\r') {
                            // Check for CRLF
                            next = *(p + 1);
                            if (next == '\n') {
                                p += 2; // Skip the '\r\n' sequence
                                break;
                            }

                            // CR
                            p++; // Skip the '\r' character
                            break;
                        }

                        // Move the pointer to the next character
                        p++;
                    }

                    continue;
                }
                // Check for group comment beginning
                else if (next == '*') {

                    // Move the pointer after the checked characters
                    p += 2;

                    // Skip until encountering a null character or a group comment ending
                    while (true) {

                        c = *p;

                        // Terminate when encountering a null character
                        if (c == '\0')
                            break; // Terminate this while loop, so it will go to the next iteration of the main loop

                        // Stop commenting when encountering the ending sequence
                        if (c == '*') {
                            next = *(p + 1);
                            if (next == '/') {
                                p += 2; // Skip the '*/' sequence
                                break;
                            }
                        }

                        // Move the pointer to the next character
                        p++;
                    }

                    continue;
                }
            }

            // Append the character to current
            current += c;

            // Move the pointer to the next character
            p++;

            // Check if the current character is a separator
            if (current.size() == 1 && isSeparator(c)) {
                tokens.emplace_back(1, c);
                current.clear();
                continue;
            }

            // Check if current is an operator
            if (isOperator(current)) {
                tokens.push_back(current);
                current.clear();
                continue;
            }

            // Get the next character
            char next = *p;

            // Go to the next iteration if the next character is NOT a separator
            if (!isspace(next) && !isSeparator(next) && !isOperator(std::string(1, next)))
                continue;

            // Check if current is a keyword
            if (isKeyword(current)) {
                tokens.push_back(current);
                current.clear();
                continue;
            }

            // Add current as an identifier
            tokens.push_back(current);
            current.clear();
        }

        if (!current.empty())
            tokens.push_back(current);
        return tokens;
    }

    std::vector<std::string> Lexer::lex(const std::string& str) {
        return lexHelper(str.c_str());
    }

} // Spark
