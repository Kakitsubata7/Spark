#include <gtest/gtest.h>

#include "frontend/lexer/token_value.hpp"

using namespace Spark::FrontEnd;

TEST(TokenValueTest, EqTest) {
    TokenValue a("foo", {1, 1}, {2, 3});
    TokenValue b("foo", {1, 1}, {2, 3});
    EXPECT_TRUE(a == b);

    a = TokenValue("bar", {3, 3}, {7, 9});
    b = TokenValue("bar", {3, 3}, {7, 9});
    EXPECT_TRUE(a == b);

    b.lexeme = "foo";
    EXPECT_FALSE(a == b);

    a = TokenValue("IbOJ6\th0_\n \rB7E", {90808990, 1237827},
        {8264291, 878732});
    b = TokenValue("\tmIYpd6\nw_z", {1878278, 128273892},
        {784949, 8738772});
    EXPECT_FALSE(a == b);

    b.lexeme = "IbOJ6\th0_\n \rB7E";
    EXPECT_FALSE(a == b);

    b.start = {90808990, 1237827};
    EXPECT_FALSE(a == b);

    b.end = {8264291, 878732};
    EXPECT_TRUE(a == b);
}

TEST(TokenValueTest, NeTest) {
    TokenValue a("foo", {1, 3}, {2, 5});
    TokenValue b("foo", {1, 3}, {2, 5});
    EXPECT_FALSE(a != b);

    a = TokenValue("bar", {3, 15}, {7, 21});
    b = TokenValue("bar", {3, 15}, {7, 21});
    EXPECT_FALSE(a != b);

    b.lexeme = "baz";
    EXPECT_TRUE(a != b);

    a = TokenValue("zCr__ DASBy0\no", {122990, 87983},
        {4991220, 3897});
    b = TokenValue("", {1, 1}, {1, 1});
    EXPECT_TRUE(a != b);

    b.lexeme = "zCr__ DASBy0\no";
    EXPECT_TRUE(a != b);

    b.start = {122990, 87983};
    EXPECT_TRUE(a != b);

    b.end = {4991220, 3897};
    EXPECT_FALSE(a != b);
}
