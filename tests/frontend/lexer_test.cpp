#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "frontend/lexer.hpp"

using namespace Spark::FrontEnd;

namespace Spark::FrontEnd {
    class LexerTests : public ::testing::Test { };

    TEST_F(LexerTests, LexWordKeywords) {
        size_t line = 1, column = 1;
        std::istringstream stream("alias");
        Result<Token> result = Lexer::lexWord(stream, line, column);
        EXPECT_FALSE(result.hasError());
        EXPECT_EQ(result.value(), Token(TokenType::Keyword, "alias", 1, 1));
        EXPECT_EQ(column, 6);

        line = 1; column = 1;
        stream = std::istringstream("as");
        result = Lexer::lexWord(stream, line, column);
        EXPECT_FALSE(result.hasError());
        EXPECT_EQ(result.value(), Token(TokenType::Keyword, "as", 1, 1));
        EXPECT_EQ(column, 3);

        line = 1; column = 1;
        stream = std::istringstream("break");
        result = Lexer::lexWord(stream, line, column);
        EXPECT_FALSE(result.hasError());
        EXPECT_EQ(result.value(), Token(TokenType::Keyword, "break", 1, 1));
        EXPECT_EQ(column, 6);

        line = 1; column = 1;
        stream = std::istringstream("_abc");
        result = Lexer::lexWord(stream, line, column);
        EXPECT_FALSE(result.hasError());
        EXPECT_EQ(result.value(), Token(TokenType::Identifier, "_abc", 1, 1));
        EXPECT_EQ(column, 5);

        line = 1; column = 1;
        stream = std::istringstream("1st");
        result = Lexer::lexWord(stream, line, column);
        EXPECT_TRUE(result.hasError());
        EXPECT_EQ(column, 1);
    }

    TEST_F(LexerTests, LexWordIdentifiers) {
        size_t line = 1, column = 1;
        std::istringstream stream("_abc123_");
        Result<Token> result = Lexer::lexWord(stream, line, column);
        EXPECT_FALSE(result.hasError());
        EXPECT_EQ(result.value(), Token(TokenType::Identifier, "_abc123_", 1, 1));
        EXPECT_EQ(column, 9);
    }

    TEST_F(LexerTests, LexWordInvalid) {
        size_t line = 1, column = 1;
        std::istringstream stream("1st");
        Result<Token> result = Lexer::lexWord(stream, line, column);
        EXPECT_TRUE(result.hasError());
        EXPECT_EQ(column, 1);
    }
}
