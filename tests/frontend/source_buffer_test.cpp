#include <gtest/gtest.h>

#include "frontend/source_buffer.hpp"

using namespace Spark::FrontEnd;

TEST(SourceBufferTest, CR) {
    SourceBuffer srcbuf;
    srcbuf.append("foo\r");
    srcbuf.append("\r");
    srcbuf.append("bar");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 3);
    EXPECT_EQ(srcbuf.getLine(1), "foo");
    EXPECT_EQ(srcbuf.getLine(2), "");
    EXPECT_EQ(srcbuf.getLine(3), "bar");

    srcbuf.clear();
    srcbuf.append("foo\rX");
    srcbuf.append("Y");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 2);
    EXPECT_EQ(srcbuf.getLine(1), "foo");
    EXPECT_EQ(srcbuf.getLine(2), "XY");

    srcbuf.clear();
    srcbuf.append("A\r");
    srcbuf.append("\rB");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 3);
    EXPECT_EQ(srcbuf.getLine(1), "A");
    EXPECT_EQ(srcbuf.getLine(2), "");
    EXPECT_EQ(srcbuf.getLine(3), "B");
}

TEST(SourceBufferTest, LF) {
    SourceBuffer srcbuf;
    srcbuf.append("hello");
    srcbuf.append("\n");
    srcbuf.append("world\n!");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 3);
    EXPECT_EQ(srcbuf.getLine(1), "hello");
    EXPECT_EQ(srcbuf.getLine(2), "world");
    EXPECT_EQ(srcbuf.getLine(3), "!");
}

TEST(SourceBufferTest, CRLF) {
    // Ends with CRLF
    SourceBuffer srcbuf;
    srcbuf.append("What did the 0 say to the 8?\r\n");
    srcbuf.append("Nice belt!\r\n");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 3);
    EXPECT_EQ(srcbuf.getLine(1), "What did the 0 say to the 8?");
    EXPECT_EQ(srcbuf.getLine(2), "Nice belt!");
    EXPECT_EQ(srcbuf.getLine(3), "");

    // Ends with no CRLF
    srcbuf.clear();
    srcbuf.append("What do you call a five foot psychic that's escaped from jail?\r\n");
    srcbuf.append("A small medium at large.");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 2);
    EXPECT_EQ(srcbuf.getLine(1), "What do you call a five foot psychic that's escaped from jail?");
    EXPECT_EQ(srcbuf.getLine(2), "A small medium at large.");

    // Mixed
    srcbuf.clear();
    srcbuf.append("1\r \n2\n\r3\r4\n5\r\n");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 8);
    EXPECT_EQ(srcbuf.getLine(1), "1");
    EXPECT_EQ(srcbuf.getLine(2), " ");
    EXPECT_EQ(srcbuf.getLine(3), "2");
    EXPECT_EQ(srcbuf.getLine(4), "");
    EXPECT_EQ(srcbuf.getLine(5), "3");
    EXPECT_EQ(srcbuf.getLine(6), "4");
    EXPECT_EQ(srcbuf.getLine(7), "5");
    EXPECT_EQ(srcbuf.getLine(8), "");

    // CRLF split in chunks
    srcbuf.clear();
    srcbuf.append("foo\r");
    srcbuf.append("\nbar");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 2);
    EXPECT_EQ(srcbuf.getLine(1), "foo");
    EXPECT_EQ(srcbuf.getLine(2), "bar");

    // CRLF split in chunks #2
    srcbuf.clear();
    srcbuf.append("a\r");
    srcbuf.append("\nb\r");
    srcbuf.append("\nc");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 3);
    EXPECT_EQ(srcbuf.getLine(1), "a");
    EXPECT_EQ(srcbuf.getLine(2), "b");
    EXPECT_EQ(srcbuf.getLine(3), "c");
}

TEST(SourceBufferTest, Mixed) {
    SourceBuffer srcbuf;

    srcbuf.append("a\r\nb\nc\rd\r\ne\n\r\nf");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 7);
    EXPECT_EQ(srcbuf.getLine(1), "a");
    EXPECT_EQ(srcbuf.getLine(2), "b");
    EXPECT_EQ(srcbuf.getLine(3), "c");
    EXPECT_EQ(srcbuf.getLine(4), "d");
    EXPECT_EQ(srcbuf.getLine(5), "e");
    EXPECT_EQ(srcbuf.getLine(6), "");
    EXPECT_EQ(srcbuf.getLine(7), "f");

    srcbuf.clear();
    srcbuf.append("\n\n");
    srcbuf.append("\r\r\n\n");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 6);
    EXPECT_EQ(srcbuf.getLine(1), "");
    EXPECT_EQ(srcbuf.getLine(2), "");
    EXPECT_EQ(srcbuf.getLine(3), "");
    EXPECT_EQ(srcbuf.getLine(4), "");
    EXPECT_EQ(srcbuf.getLine(5), "");
    EXPECT_EQ(srcbuf.getLine(6), "");
}

TEST(SourceBufferTest, EmptyChunks) {
    SourceBuffer srcbuf;
    srcbuf.append("");
    srcbuf.append("foo\n");
    srcbuf.append("");
    srcbuf.append("bar");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 2);
    EXPECT_EQ(srcbuf.getLine(1), "foo");
    EXPECT_EQ(srcbuf.getLine(2), "bar");
}

TEST(SourceBufferTest, EndsWithNewline) {
    SourceBuffer srcbuf;
    srcbuf.append("foobarbaz\n");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 2);
    EXPECT_EQ(srcbuf.getLine(1), "foobarbaz");
    EXPECT_EQ(srcbuf.getLine(2), "");

    srcbuf.clear();
    srcbuf.append("What did one snowman say to the other snowman?\r");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 2);
    EXPECT_EQ(srcbuf.getLine(1), "What did one snowman say to the other snowman?");
    EXPECT_EQ(srcbuf.getLine(2), "");

    srcbuf.clear();
    srcbuf.append("Smells like carrots.\r\n");
    srcbuf.flush();
    EXPECT_EQ(srcbuf.lineNum(), 2);
    EXPECT_EQ(srcbuf.getLine(1), "Smells like carrots.");
    EXPECT_EQ(srcbuf.getLine(2), "");
}
