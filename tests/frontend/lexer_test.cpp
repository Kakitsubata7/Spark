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
        EXPECT_EQ(line, 1); EXPECT_EQ(column, 6);

        line = 1; column = 1;
        stream = std::istringstream("as");
        result = Lexer::lexWord(stream, line, column);
        EXPECT_FALSE(result.hasError());
        EXPECT_EQ(result.value(), Token(TokenType::Keyword, "as", 1, 1));
        EXPECT_EQ(line, 1); EXPECT_EQ(column, 3);

        line = 1; column = 1;
        stream = std::istringstream("break");
        result = Lexer::lexWord(stream, line, column);
        EXPECT_FALSE(result.hasError());
        EXPECT_EQ(result.value(), Token(TokenType::Keyword, "break", 1, 1));
        EXPECT_EQ(line, 1); EXPECT_EQ(column, 6);
    }

    TEST_F(LexerTests, LexWordIdentifiers) {
        size_t line = 1, column = 1;
        std::istringstream stream("_abc123_");
        Result<Token> result = Lexer::lexWord(stream, line, column);
        EXPECT_FALSE(result.hasError());
        EXPECT_EQ(result.value(), Token(TokenType::Identifier, "_abc123_", 1, 1));
        EXPECT_EQ(line, 1); EXPECT_EQ(column, 9);
    }

    TEST_F(LexerTests, LexWordInvalid) {
        size_t line = 1, column = 1;
        std::istringstream stream("1st");
        Result<Token> result = Lexer::lexWord(stream, line, column);
        EXPECT_TRUE(result.hasError());
        EXPECT_EQ(line, 1); EXPECT_EQ(column, 1);
    }

    TEST_F(LexerTests, BinaryNumberValid) {
        size_t line = 1, column = 1;
        std::istringstream stream("0b0101");
        Result<Token> result = Lexer::lexBinaryNumber(stream, line, column);
        EXPECT_FALSE(result.hasError());
        EXPECT_EQ(result.value(), Token(TokenType::Number, "0b0101", 1, 1));
        EXPECT_EQ(line, 1); EXPECT_EQ(column, 7);

        line = 1; column = 1;
        stream = std::istringstream("0B1100010100010101");
        result = Lexer::lexBinaryNumber(stream, line, column);
        EXPECT_FALSE(result.hasError());
        std::cout << result.value().lexeme << std::endl;
        EXPECT_EQ(result.value(), Token(TokenType::Number, "0B1100010100010101", 1, 1));
        EXPECT_EQ(line, 1); EXPECT_EQ(column, 19);
    }
}
