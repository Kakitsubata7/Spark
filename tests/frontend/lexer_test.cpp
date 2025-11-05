#include <sstream>

#include <gtest/gtest.h>

#include "frontend/lexer.hpp"

using namespace Spark::FrontEnd;

static void ExpectTokens(Lexer& lexer, const std::vector<Token>& expected) {
    for (size_t i = 0; i < expected.size(); ++i) {
        Token actual = lexer.lex();
        if (actual != expected[i]) {
            std::ostringstream oss;
            oss << "Token mismatch at index " << i << "\n"
                << "Expected: " << expected[i] << "\n"
                << "Actual:   " << actual << "\n";
            FAIL() << oss.str();
        }
        EXPECT_EQ(actual, expected[i]);
    }
    Token eof = lexer.lex();
    EXPECT_EQ(eof.type, TokenType::EndOfFile);
}

TEST(LexerTest, SimpleCase1) {
    std::string source = R"(let x = 1 if x > 0 do x = x + 1 end)";
    std::istringstream iss(source);

    Lexer lexer(iss);
    EXPECT_EQ(lexer.lex(), Token(TokenType::Let, {"let", 1, 1}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Identifier, {"x", 1, 5}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Assign, {"=", 1, 7}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Integer, {"1", 1, 9}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::If, {"if", 1, 11}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Identifier, {"x", 1, 14}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Gt, {">", 1, 16}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Integer, {"0", 1, 18}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Do, {"do", 1, 20}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Identifier, {"x", 1, 23}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Assign, {"=", 1, 25}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Identifier, {"x", 1, 27}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Add, {"+", 1, 29}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::Integer, {"1", 1, 31}));
    EXPECT_EQ(lexer.lex(), Token(TokenType::End, {"end", 1, 33}));
}
