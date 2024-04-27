#include "Lexer.hpp"

#include <cctype>

namespace Spark {

    /* ===== Operations ===== */

    std::vector<Token> Lexer::lex(const std::string& str) {
        return Lexer(str).lex();
    }

    std::vector<Token> Lexer::lex() {
        std::vector<Token> tokens;

        std::string current;

        int c;
        while ((c = iss.get()) != EOF) {
            // Check for line commenting end
            if (commentState == CommentState::LineCommenting) {
                // Check for LF
                if (c == '\n')
                    commentState = CommentState::None;
                // Check for CR and CRLF
                else if (c == '\r') {
                    c = iss.peek();
                    if (c == EOF)
                        break;
                    if (c == '\n')
                        iss.ignore(1);

                    commentState = CommentState::None;
                }

                continue;
            }
            // Check for group commenting end
            else if (commentState == CommentState::GroupCommenting) {
                if (c == '*') {
                    c = iss.peek();
                    if (c == EOF)
                        break;

                    if (c == '/') {
                        commentState = CommentState::None;
                        iss.ignore(1);
                    }
                }

                continue;
            }

            // Ignore white space characters
            if (isspace(c))
                continue;

            // Check for comment beginnings
            if (c == '/') {
                c = iss.peek();
                if (c == EOF)
                    break;

                // Check for line comment beginning
                if (c == '/') {
                    commentState = CommentState::LineCommenting;
                    iss.ignore(1);
                }
                // Check for group comment beginning
                else if (c == '*') {
                    commentState = CommentState::GroupCommenting;
                    iss.ignore(1);
                }

                continue;
            }

            current += static_cast<char>(c);
        }

        if (!current.empty())
            tokens.emplace_back(TokenType::Identifier, current);

        return tokens;
    }

} // Spark
