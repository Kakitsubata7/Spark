#include <gtest/gtest.h>

#include "frontend/lexer/token_value.hpp"

using namespace Spark::FrontEnd;

TEST(TokenValueTest, EqTest) {
    TokenValue a("foo", 1, 2);
    TokenValue b("foo", 1, 2);
    EXPECT_TRUE(a == b);

    a = TokenValue("bar", 3, 7);
    b = TokenValue("bar", 3, 7);
    EXPECT_TRUE(a == b);

    b.lexeme = "foo";
    EXPECT_FALSE(a == b);

    a = TokenValue("IbOJ6\th0_\n \rB7E", 90808990, 8264291);
    b = TokenValue("\tmIYpd6\nw_z", 1878278, 784949);
    EXPECT_FALSE(a == b);

    b.lexeme = "IbOJ6\th0_\n \rB7E";
    EXPECT_FALSE(a == b);

    b.line = 90808990;
    EXPECT_FALSE(a == b);

    b.column = 8264291;
    EXPECT_TRUE(a == b);
}

TEST(TokenValueTest, NeTest) {
    TokenValue a("foo", 1, 2);
    TokenValue b("foo", 1, 2);
    EXPECT_FALSE(a != b);

    a = TokenValue("bar", 3, 7);
    b = TokenValue("bar", 3, 7);
    EXPECT_FALSE(a != b);

    b.lexeme = "baz";
    EXPECT_TRUE(a != b);

    a = TokenValue("zCr__ DASBy0\no", 122990, 4991220);
    b = TokenValue("", 1, 1);
    EXPECT_TRUE(a != b);

    b.lexeme = "zCr__ DASBy0\no";
    EXPECT_TRUE(a != b);

    b.line = 122990;
    EXPECT_TRUE(a != b);

    b.column = 4991220;
    EXPECT_FALSE(a != b);
}
