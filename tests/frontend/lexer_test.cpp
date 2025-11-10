#include <sstream>

#include <gtest/gtest.h>

#include "frontend/lexer.hpp"

#include "utils.hpp"

using namespace Spark::FrontEnd;
using namespace Spark::Test::FrontEnd;
using TT = TokenType;

static void expectTokens(const std::vector<Token>& actual, const std::vector<Token>& expected) {
    EXPECT_EQ(actual.size(), expected.size()) << "Number of tokens mismatched\n";
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] != expected[i]) {
            std::ostringstream oss;
            oss << "Token mismatch at index " << i << "\n"
                << "Expected: " << expected[i] << "\n"
                << "Actual:   " << actual[i] << "\n";
            FAIL() << oss.str();
        }
        EXPECT_EQ(actual[i], expected[i]);
    }
}

static Lexer testLexAll(std::string_view source, const std::vector<Token>& expectedTokens) {
    std::istringstream iss{std::string(source)};
    Lexer lexer(iss);
    expectTokens(lexer.lexAll(), expectedTokens);
    return lexer;
}

TEST(LexerTest, GeneralTest1) {
    std::string_view source = R"(let x = 1 if x > 0 do x = x + 1 end)";
    testLexAll(source, {
        {TT::Let, {"let", 1, 1}},
        {TT::Identifier, {"x", 1, 5}},
        {TT::Assign, {"=", 1, 7}},
        {TT::Integer, {"1", 1, 9}},
        {TT::If, {"if", 1, 11}},
        {TT::Identifier, {"x", 1, 14}},
        {TT::Gt, {">", 1, 16}},
        {TT::Integer, {"0", 1, 18}},
        {TT::Do, {"do", 1, 20}},
        {TT::Identifier, {"x", 1, 23}},
        {TT::Assign, {"=", 1, 25}},
        {TT::Identifier, {"x", 1, 27}},
        {TT::Add, {"+", 1, 29}},
        {TT::Integer, {"1", 1, 31}},
        {TT::End, {"end", 1, 33}}
    });
}

TEST(LexerTest, GeneralTest2) {
    std::string_view source = R"(
@cstruct
struct Vector2 do
    let x: Real
    let y: Real
end
)";
    testLexAll(source, {
        {TT::At, {"@", 2, 1}},
        {TT::Identifier, {"cstruct", 2, 2}},
        {TT::Struct, {"struct", 3, 1}},
        {TT::Identifier, {"Vector2", 3, 8}},
        {TT::Do, {"do", 3, 16}},
        {TT::Let, {"let", 4, 5}},
        {TT::Identifier, {"x", 4, 9}},
        {TT::Colon, {":", 4, 10}},
        {TT::Identifier, {"Real", 4, 12}},
        {TT::Let, {"let", 5, 5}},
        {TT::Identifier,  {"y", 5, 9}},
        {TT::Colon,  {":", 5, 10}},
        {TT::Identifier,  {"Real", 5, 12}},
        Token{TT::End,  {"end", 6, 1}}
    });
}

TEST(LexerTest, GeneralTest3) {
    std::string_view source = R"(
fn binarySearch(l: List<Int>, target: Int) -> Int do
    let low = 0
    let high = l.length() - 1
    while low <= high do
        let mid = (low + high) / 2
        if l[mid] == target do
            return mid
        else if l[mid] < target do
            low = mid + 1
        else do
            high = mid - 1
        end
    end
    return -1
end
)";
    testLexAll(source, {
        {TT::Fn, {"fn", 2, 1}},
        {TT::Identifier, {"binarySearch", 2, 4}},
        {TT::LParen, {"(", 2, 16}},
        {TT::Identifier, {"l", 2, 17}},
        {TT::Colon, {":", 2, 18}},
        {TT::Identifier, {"List", 2, 20}},
        {TT::Lt, {"<", 2, 24}},
        {TT::Identifier, {"Int", 2, 25}},
        {TT::Gt, {">", 2, 28}},
        {TT::Comma, {",", 2, 29}},
        {TT::Identifier, {"target", 2, 31}},
        {TT::Colon, {":", 2, 37}},
        {TT::Identifier, {"Int", 2, 39}},
        {TT::RParen, {")", 2, 42}},
        {TT::Arrow, {"->", 2, 44}},
        {TT::Identifier, {"Int", 2, 47}},
        {TT::Do, {"do", 2, 51}},
        {TT::Let, {"let", 3, 5}},
        {TT::Identifier, {"low", 3, 9}},
        {TT::Assign, {"=", 3, 13}},
        {TT::Integer, {"0", 3, 15}},
        {TT::Let, {"let", 4, 5}},
        {TT::Identifier, {"high", 4, 9}},
        {TT::Assign, {"=", 4, 14}},
        {TT::Identifier, {"l", 4, 16}},
        {TT::Dot, {".", 4, 17}},
        {TT::Identifier, {"length", 4, 18}},
        {TT::LParen, {"(", 4, 24}},
        {TT::RParen, {")", 4, 25}},
        {TT::Sub, {"-", 4, 27}},
        {TT::Integer, {"1", 4, 29}},
        {TT::While, {"while", 5, 5}},
        {TT::Identifier, {"low", 5, 11}},
        {TT::Le, {"<=", 5, 15}},
        {TT::Identifier, {"high", 5, 18}},
        {TT::Do, {"do", 5, 23}},
        {TT::Let, {"let", 6, 9}},
        {TT::Identifier, {"mid", 6, 13}},
        {TT::Assign, {"=", 6, 17}},
        {TT::LParen, {"(", 6, 19}},
        {TT::Identifier, {"low", 6, 20}},
        {TT::Add, {"+", 6, 24}},
        {TT::Identifier, {"high", 6, 26}},
        {TT::RParen, {")", 6, 30}},
        {TT::Div, {"/", 6, 32}},
        {TT::Integer, {"2", 6, 34}},
        {TT::If, {"if", 7, 9}},
        {TT::Identifier, {"l", 7, 12}},
        {TT::LBracket, {"[", 7, 13}},
        {TT::Identifier, {"mid", 7, 14}},
        {TT::RBracket, {"]", 7, 17}},
        {TT::Eq, {"==", 7, 19}},
        {TT::Identifier, {"target", 7, 22}},
        {TT::Do, {"do", 7, 29}},
        {TT::Return, {"return", 8, 13}},
        {TT::Identifier, {"mid", 8, 20}},
        {TT::Else, {"else", 9, 9}},
        {TT::If, {"if", 9, 14}},
        {TT::Identifier, {"l", 9, 17}},
        {TT::LBracket, {"[", 9, 18}},
        {TT::Identifier, {"mid", 9, 19}},
        {TT::RBracket, {"]", 9, 22}},
        {TT::Lt, {"<", 9, 24}},
        {TT::Identifier, {"target", 9, 26}},
        {TT::Do, {"do", 9, 33}},
        {TT::Identifier, {"low", 10, 13}},
        {TT::Assign, {"=", 10, 17}},
        {TT::Identifier, {"mid", 10, 19}},
        {TT::Add, {"+", 10, 23}},
        {TT::Integer, {"1", 10, 25}},
        {TT::Else, {"else", 11, 9}},
        {TT::Do, {"do", 11, 14}},
        {TT::Identifier, {"high", 12, 13}},
        {TT::Assign, {"=", 12, 18}},
        {TT::Identifier, {"mid", 12, 20}},
        {TT::Sub, {"-", 12, 24}},
        {TT::Integer, {"1", 12, 26}},
        {TT::End, {"end", 13, 9}},
        {TT::End, {"end", 14, 5}},
        {TT::Return, {"return", 15, 5}},
        {TT::Sub, {"-", 15, 12}},
        {TT::Integer, {"1", 15, 13}},
        {TT::End, {"end", 16, 1}}
    });
}

TEST(LexerTest, GeneralTest4) {
    std::string source = R"(
// This is a line comment
let a = "hello"
let b = /*
    This
    is
    a
    block
    comment
*/ 'world'
let c = "hello 'world'"
let d = 'hello "world"' /* This is another block comment */
)";
    removeCarriageReturns(source);
    testLexAll(source, {
        {TT::LineComment, {" This is a line comment", 2, 1}},
        {TT::Let, {"let", 3, 1}},
        {TT::Identifier, {"a", 3, 5}},
        {TT::Assign, {"=", 3, 7}},
        {TT::String, {"hello", 3, 9}},
        {TT::Let, {"let", 4, 1}},
        {TT::Identifier, {"b", 4, 5}},
        {TT::Assign, {"=", 4, 7}},
        {TT::BlockComment, {"\n    This\n    is\n    a\n    block\n    comment\n",
            4, 9}},
        {TT::String, {"world", 10, 4}},
        {TT::Let, {"let", 11, 1}},
        {TT::Identifier, {"c", 11, 5}},
        {TT::Assign, {"=", 11, 7}},
        {TT::String, {"hello 'world'", 11, 9}},
        {TT::Let, {"let", 12, 1}},
        {TT::Identifier, {"d", 12, 5}},
        {TT::Assign, {"=", 12, 7}},
        {TT::String, {"hello \"world\"", 12, 9}},
        {TT::BlockComment, {" This is another block comment ", 12, 25}}
    });
}

TEST(LexerTest, GeneralTest5) {
    std::string source = R"(
fn^ foo(n: Int) -> Int do
    const msg = "Hello /* This is not a comment */ world"
    const quote = 'What\'s more amazing than a talking dog? A spelling "bee"!'

    // For loop
    for i in 0..<n do
        print(msg)
        print(quote)
    end

    /* This is a
       block comment! */
    return n * n
end
)";
    removeCarriageReturns(source);
    testLexAll(source, {
        {TT::Fn, {"fn", 2, 1}},
        {TT::BitXor, {"^", 2, 3}},
        {TT::Identifier, {"foo", 2, 5}},
        {TT::LParen, {"(", 2, 8}},
        {TT::Identifier, {"n", 2, 9}},
        {TT::Colon, {":", 2, 10}},
        {TT::Identifier, {"Int", 2, 12}},
        {TT::RParen, {")", 2, 15}},
        {TT::Arrow, {"->", 2, 17}},
        {TT::Identifier, {"Int", 2, 20}},
        {TT::Do, {"do", 2, 24}},

        {TT::Const, {"const", 3, 5}},
        {TT::Identifier, {"msg", 3, 11}},
        {TT::Assign, {"=", 3, 15}},
        {TT::String, {"Hello /* This is not a comment */ world", 3, 17}},

        {TT::Const, {"const", 4, 5}},
        {TT::Identifier, {"quote", 4, 11}},
        {TT::Assign, {"=", 4, 17}},
        {TT::String, {"What's more amazing than a talking dog? A spelling \"bee\"!", 4, 19}},

        {TT::LineComment, {" For loop", 6, 5}},

        {TT::For, {"for", 7, 5}},
        {TT::Identifier, {"i", 7, 9}},
        {TT::In, {"in", 7, 11}},
        {TT::Integer, {"0", 7, 14}},
        {TT::RangeExcl, {"..<", 7, 15}},
        {TT::Identifier, {"n", 7, 18}},
        {TT::Do, {"do", 7, 20}},

        {TT::Identifier, {"print", 8, 9}},
        {TT::LParen, {"(", 8, 14}},
        {TT::Identifier, {"msg", 8, 15}},
        {TT::RParen, {")", 8, 18}},

        {TT::Identifier, {"print", 9, 9}},
        {TT::LParen, {"(", 9, 14}},
        {TT::Identifier, {"quote", 9, 15}},
        {TT::RParen, {")", 9, 20}},

        {TT::End, {"end", 10, 5}},

        {TT::BlockComment, {" This is a\n       block comment! ", 12, 5}},

        {TT::Return, {"return", 14, 5}},
        {TT::Identifier, {"n", 14, 12}},
        {TT::Mul, {"*", 14, 14}},
        {TT::Identifier, {"n", 14, 16}},
        {TT::End, {"end", 15, 1}}
    });
}
