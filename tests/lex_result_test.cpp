#include <gtest/gtest.h>

#include "compiler/lex_error.hpp"
#include "compiler/lex_result.hpp"
#include "compiler/token.hpp"

using namespace spark;

TEST(LexResultTest, DefaultConstructor) {
    LexResult result;
    EXPECT_TRUE(result.tokens().empty());
    EXPECT_TRUE(result.errors().empty());
    EXPECT_TRUE(result.success());
}

TEST(LexResultTest, PushToken) {
    LexResult result;

    Token token1(TokenType::Identifier, "variable", 3, 5);
    Token token2(TokenType::Integer, "42", 7, 2);

    result.pushToken(token1);
    result.pushToken(std::move(token2));

    const std::vector<Token>& tokens = result.tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].lexeme, "variable");
    EXPECT_EQ(tokens[1].lexeme, "42");
}

TEST(LexResultTest, PushError) {
    LexResult result;

    LexError error1("unexpected character", 5, 12);
    LexError error2("unterminated string literal", 8, 20);

    result.pushError(error1);
    result.pushError(std::move(error2));

    const std::vector<LexError>& errors = result.errors();
    ASSERT_EQ(errors.size(), 2);
    EXPECT_EQ(errors[0].message, "unexpected character");
    EXPECT_EQ(errors[1].message, "unterminated string literal");
}

TEST(LexResultTest, SuccessFunction) {
    LexResult result;

    EXPECT_TRUE(result.success());

    result.pushError(LexError("syntax error", 10, 3));
    EXPECT_FALSE(result.success());
}

TEST(LexResultTest, EdgeCases) {
    LexResult result;
    const std::vector<Token>& tokens = result.tokens();
    const std::vector<LexError>& errors = result.errors();

    // Add empty token
    result.pushToken(Token{TokenType::Identifier, "", 1, 1});
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].lexeme, "");

    // Add empty error message
    result.pushError(LexError("", 2, 5));
    EXPECT_EQ(errors.size(), 1);
    EXPECT_EQ(errors[0].message, "");

    // Multiple additions
    for (size_t i = 0; i < 10; ++i) {
        result.pushToken({TokenType::Integer, std::to_string(i), i, i});
        result.pushError({"Error " + std::to_string(i), i, i});
    }
    EXPECT_EQ(tokens.size(), 11);
    EXPECT_EQ(errors.size(), 11);
}
