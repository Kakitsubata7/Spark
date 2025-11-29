#include <gtest/gtest.h>

#include <string>
#include <string_view>

#include "frontend/source_buffer.hpp"

using namespace Spark::FrontEnd;

static void testLoad(std::string_view sv, const std::vector<std::string>& expected) {
    std::istringstream iss{std::string(sv)};
    SourceBuffer srcbuf(iss);
    EXPECT_EQ(srcbuf.lineNum(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(srcbuf.getLine(i + 1), expected[i]);
    }
}

TEST(SourceBufferTest, CR) {
    testLoad("foo\r\rbar", {
        "foo", "", "bar"
    });

    testLoad("foo\rX", {
        "foo", "X"
    });

    testLoad("A\r\rB", {
        "A", "", "B"
    });
}

TEST(SourceBufferTest, LF) {
    testLoad("hello\nworld\n!", {
        "hello", "world", "!"
    });
}

TEST(SourceBufferTest, CRLF) {
    // Ends with CRLF
    testLoad("What did the 0 say to the 8?\r\nNice belt!\r\n", {
        "What did the 0 say to the 8?", "Nice belt!", ""
    });

    // Ends with no CRLF
    testLoad("What do you call a five foot psychic that's escaped from jail?\r\nA small medium at large.", {
        "What do you call a five foot psychic that's escaped from jail?", "A small medium at large."
    });

    // Mixed
    testLoad("1\r \n2\n\r3\r4\n5\r\n", {
        "1", " ", "2", "", "3", "4", "5", ""
    });

    // CRLF
    testLoad("foo\r\nbar", {
        "foo", "bar"
    });

    // CRLF #2
    testLoad("a\r\nb\r\nc", {
        "a", "b", "c"
    });
}

TEST(SourceBufferTest, Mixed) {
    testLoad("a\r\nb\nc\rd\r\ne\n\r\nf", {
        "a", "b", "c", "d", "e", "", "f"
    });

    testLoad("\n\n\r\r\n\n", {
        "", "", "", "", "", ""
    });
}

TEST(SourceBufferTest, Empty) {
    testLoad("", {
        ""
    });
}

TEST(SourceBufferTest, EndsWithNewline) {
    testLoad("foobarbaz\n", {
        "foobarbaz", ""
    });

    testLoad("What did one snowman say to the other snowman?\r", {
        "What did one snowman say to the other snowman?", ""
    });

    testLoad("Smells like carrots.\r\n", {
        "Smells like carrots.", ""
    });
}

TEST(SourceBufferTest, NoNewline) {
    testLoad("foobar", {
        "foobar"
    });
}

TEST(SourceBufferTest, ReadChunk) {
    std::istringstream iss("abc\r\ndef");
    SourceBuffer srcbuf(iss);
    constexpr size_t maxSize = 4;
    char buf[maxSize];
    size_t read = 0;

    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, maxSize);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("abc\n"));

    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, 3);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("def"));

    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, 0);


    iss = std::istringstream("foobar\nbaz");
    srcbuf = SourceBuffer(iss);
    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, maxSize);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("foob"));

    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, maxSize);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("ar\nb"));

    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, 2);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("az"));

    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, 0);


    iss = std::istringstream("foobarbaz");
    srcbuf = SourceBuffer(iss);
    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, maxSize);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("foob"));

    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, maxSize);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("arba"));

    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, 1);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("z"));

    read = srcbuf.readChunk(buf, maxSize);
    EXPECT_EQ(read, 0);
}
