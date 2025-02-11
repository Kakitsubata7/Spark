#include <sstream>
#include <string_view>

#include <gtest/gtest.h>

#include "compiler/lex_error.hpp"

using namespace spark;

TEST(LexErrorTest, Constructor) {
    LexError error("unexpected character", 5, 12);
    EXPECT_EQ(error.message, "unexpected character");
    EXPECT_EQ(error.line, 5);
    EXPECT_EQ(error.column, 12);
}

TEST(LexErrorTest, StreamOutput) {
    auto test = [](const LexError& error, std::string_view expected) {
        std::ostringstream os;
        os << error;
        EXPECT_EQ(os.str(), expected);
    };

    test({"unexpected token", 3, 14}, "3:14: unexpected token");
    test({"invalid string literal", 10, 2}, "10:2: invalid string literal");
    test({"unterminated comment", 7, 25}, "7:25: unterminated comment");
}

TEST(LexErrorTest, StreamOutputEdgeCases) {
    // Empty message
    LexError emptyMessageError("", 1, 1);
    std::ostringstream os1;
    os1 << emptyMessageError;
    EXPECT_EQ(os1.str(), "1:1: ");

    // Line and column at zero
    LexError zeroPositionError("error at start", 0, 0);
    std::ostringstream os2;
    os2 << zeroPositionError;
    EXPECT_EQ(os2.str(), "0:0: error at start");

    // Large line/column numbers
    LexError largePositionError("end of file reached", 99999, 888);
    std::ostringstream os3;
    os3 << largePositionError;
    EXPECT_EQ(os3.str(), "99999:888: end of file reached");
}
