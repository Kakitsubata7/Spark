#include <string_view>

#include <gtest/gtest.h>

#include "frontend/lexer.hpp"
#include "frontend/parser/token_producer.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

std::string_view test1Code = "let x = 1";

void test1(TokenProducer& producer) {
    Token let(TokenType::Let, "let", {1, 1}, {1, 3});
    Token x(TokenType::Identifier, "x", {1, 5}, {1, 5});
    Token assign(TokenType::Assign, "=", {1, 7}, {1, 7});
    Token one(TokenType::Integer, "1", {1, 9}, {1, 9});
    Token eof(TokenType::EndOfFile, "", {1, 9}, {1, 9});

    EXPECT_EQ(producer.peek(), let);
    EXPECT_EQ(producer.peek(), let);
    EXPECT_EQ(producer.next(), let);
    EXPECT_EQ(producer.next(), x);
    EXPECT_EQ(producer.peek(), assign);
    EXPECT_EQ(producer.next(), assign);
    EXPECT_EQ(producer.next(), one);
    EXPECT_EQ(producer.peek(), eof);
    EXPECT_EQ(producer.next(), eof);
}

TEST(TokenProducerTest, Test1) {
    {
        Lexer lexer(test1Code);
        SimpleTokenProducer producer(lexer);
        test1(producer);
    }

    {
        Lexer lexer(test1Code);
        RewindTokenProducer producer(lexer);
        test1(producer);
    }
}

std::string_view test2Code = "if foo() then x + 1 else x";

void test2(TokenProducer& producer) {
    Token tok1(TokenType::If, "if", {1, 1}, {1, 2});
    Token tok2(TokenType::Identifier, "foo", {1, 4}, {1, 6});
    Token tok3(TokenType::LParen, "(", {1, 7}, {1, 7});
    Token tok4(TokenType::RParen, ")", {1, 8}, {1, 8});
    Token tok5(TokenType::Then, "then", {1, 10}, {1, 13});
    Token tok6(TokenType::Identifier, "x", {1, 15}, {1, 15});
    Token tok7(TokenType::Add, "+", {1, 17}, {1, 17});
    Token tok8(TokenType::Integer, "1", {1, 19}, {1, 19});
    Token tok9(TokenType::Else, "else", {1, 21}, {1, 24});
    Token tok10(TokenType::Identifier, "x", {1, 26}, {1, 26});
    Token eof(TokenType::EndOfFile, "", {1, 26}, {1, 26});

    EXPECT_EQ(producer.peek(), tok1);
    EXPECT_EQ(producer.next(), tok1);

    EXPECT_EQ(producer.next(), tok2);
    EXPECT_EQ(producer.next(), tok3);
    EXPECT_EQ(producer.next(), tok4);

    EXPECT_EQ(producer.peek(), tok5);
    EXPECT_EQ(producer.next(), tok5);

    EXPECT_EQ(producer.next(), tok6);
    EXPECT_EQ(producer.next(), tok7);
    EXPECT_EQ(producer.next(), tok8);

    EXPECT_EQ(producer.peek(), tok9);
    EXPECT_EQ(producer.peek(), tok9);
    EXPECT_EQ(producer.peek(), tok9);
    EXPECT_EQ(producer.next(), tok9);

    EXPECT_EQ(producer.next(), tok10);

    EXPECT_EQ(producer.peek(), eof);
    EXPECT_EQ(producer.next(), eof);
    EXPECT_EQ(producer.next(), eof);
    EXPECT_EQ(producer.peek(), eof);
}

TEST(TokenProducerTest, Test2) {
    {
        Lexer lexer(test2Code);
        SimpleTokenProducer producer(lexer);
        test2(producer);
    }

    {
        Lexer lexer(test2Code);
        RewindTokenProducer producer(lexer);
        test2(producer);
    }
}

TEST(RewindTokenProducerTest, Test1) {
    Lexer lexer("a b c d e");
    RewindTokenProducer producer(lexer);

    Token a(TokenType::Identifier, "a", {1, 1}, {1, 1});
    Token b(TokenType::Identifier, "b", {1, 3}, {1, 3});
    Token c(TokenType::Identifier, "c", {1, 5}, {1, 5});
    Token d(TokenType::Identifier, "d", {1, 7}, {1, 7});
    Token e(TokenType::Identifier, "e", {1, 9}, {1, 9});

    EXPECT_EQ(producer.next(), a);
    EXPECT_EQ(producer.peek(), b);
    EXPECT_EQ(producer.next(), b);

    producer.rewind();
    EXPECT_EQ(producer.next(), a);
    EXPECT_EQ(producer.next(), b);
    EXPECT_EQ(producer.next(), c);
    EXPECT_EQ(producer.next(), d);

    producer.rewind();
    EXPECT_EQ(producer.next(), a);
    EXPECT_EQ(producer.next(), b);
    EXPECT_EQ(producer.next(), c);
    EXPECT_EQ(producer.next(), d);
    EXPECT_EQ(producer.peek(), e);

    producer.rewind();
    EXPECT_EQ(producer.next(), a);
    EXPECT_EQ(producer.next(), b);
    EXPECT_EQ(producer.next(), c);
    EXPECT_EQ(producer.next(), d);
    EXPECT_EQ(producer.next(), e);
}
