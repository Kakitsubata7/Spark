#include <sstream>
#include <string_view>

#include <gtest/gtest.h>

#include "compiler/token.hpp"

using namespace spark;

void test(const Token& token, std::string_view expected) {
    std::ostringstream os;
    os << token;
    EXPECT_EQ(os.str(), expected);
}

TEST(TokenTest, StreamOutputBasic) {
    test({TokenType::Identifier, "foo", 2, 5},
         "<Identifier: 'foo' 2:5>");

    test({TokenType::Keyword, "if", 3, 1},
         "<Keyword: 'if' 3:1>");

    test({TokenType::Symbol, "+", 1, 8},
         "<Symbol: '+' 1:8>");

    test({TokenType::Integer, "37", 3, 7},
         "<Integer: '37' 3:7>");

    test({TokenType::Real, "3.14", 7, 10},
         "<Real: '3.14' 7:10>");

    test({TokenType::Boolean, "true", 1, 2},
         "<Boolean: 'true' 1:2>");

    test({TokenType::String, "Hello, World!", 4, 3},
         R"(<String: 'Hello, World!' 4:3>)");

    test({TokenType::Nil, "nil", 30, 40},
         "<Nil 30:40>");

    test({TokenType::Discard, "_", 5, 6},
         "<Discard 5:6>");

    test({TokenType::Comment, "Comment", 0, 0},
         "<Comment: 'Comment' 0:0>");

    test({TokenType::End, "\0", 20, 1},
         "<End 20:1>");

    test({TokenType::Unknown, "unknown", 100, 200},
         "<Unknown: 'unknown' 100:200>");
}

TEST(TokenTest, StreamOutputSpecialCharacters) {
    // Token with spaces
    test({TokenType::Identifier, "with space", 5, 12},
         "<Identifier: 'with space' 5:12>");

    // Token with escape sequences
    test({TokenType::String, "New\nLine", 6, 2},
         "<String: 'New\nLine' 6:2>");
    test({TokenType::String, "Tab\tSeparated", 8, 4},
         "<String: 'Tab\tSeparated' 8:4>");
}

TEST(TokenTest, StreamOutputEdgeCases) {
    // Empty lexeme
    test({TokenType::Identifier, "", 1, 1},
         "<Identifier: '' 1:1>");

    // Multi-line string
    test({TokenType::String, "Line1\nLine2", 12, 5},
         "<String: 'Line1\nLine2' 12:5>");

    // Tokens at extreme positions
    test({TokenType::Keyword, "return", 9999, 999},
         "<Keyword: 'return' 9999:999>");
    test({TokenType::Symbol, "&", 0, 0},
         "<Symbol: '&' 0:0>");
}

TEST(TokenTest, StreamOutputLongLexeme) {
    std::string longLexeme(100, 'a'); // "aaaaaaaaaa... (100 times)"
    std::ostringstream expected;
    expected << "<Identifier: '" << longLexeme << "' 50:25>";
    test({TokenType::Identifier, longLexeme, 50, 25},
         expected.str());
}

