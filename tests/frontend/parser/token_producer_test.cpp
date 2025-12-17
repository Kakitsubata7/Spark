#include <gtest/gtest.h>

#include "frontend/lexer.hpp"
#include "frontend/parser/token_producer.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

TEST(LexerTokenProducerTest, Test1) {
    Lexer lexer("let x = 1");
    LexerTokenProducer producer(lexer);

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

TEST(RewindLexerTokenProducerTest, Test1) {

}
