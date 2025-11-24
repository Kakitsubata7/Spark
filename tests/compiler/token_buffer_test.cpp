#include <gtest/gtest.h>

#include "frontend/lexer/token_buffer.hpp"

using namespace Spark::FrontEnd;

TEST(TokenBufferTest, AppendTest) {
    TokenBuffer buffer(3, 7);

    buffer.append('a');
    buffer.append('b');
    buffer.append('c');
    EXPECT_EQ(buffer.view(), "abc");
    EXPECT_EQ(buffer.line(), 3);
    EXPECT_EQ(buffer.column(), 7);

    buffer.append("_123");
    EXPECT_EQ(buffer.view(), "abc_123");
    EXPECT_EQ(buffer.line(), 3);
    EXPECT_EQ(buffer.column(), 7);

    buffer.reset(103, 890);
    buffer.append("\r\nfoo");
    EXPECT_EQ(buffer.view(), "\r\nfoo");
    EXPECT_EQ(buffer.line(), 103);
    EXPECT_EQ(buffer.column(), 890);
}
