#include <gtest/gtest.h>

#include "frontend/source_buffer.hpp"
#include "frontend/source_reader.hpp"

using namespace Spark::FrontEnd;

TEST(SourceBufferTest, ReadChunk) {
    std::istringstream iss("abc\r\ndef");
    SourceBuffer srcbuf(iss);
    SourceReader reader(srcbuf);
    constexpr size_t maxSize = 4;
    char buf[maxSize];
    size_t read = 0;

    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, maxSize);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("abc\r"));

    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, 4);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("\ndef"));

    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, 0);


    iss = std::istringstream("foobar\nbaz");
    srcbuf = SourceBuffer(iss);
    reader = SourceReader(srcbuf);
    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, maxSize);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("foob"));

    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, maxSize);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("ar\nb"));

    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, 2);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("az"));

    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, 0);


    iss = std::istringstream("foobarbaz");
    srcbuf = SourceBuffer(iss);
    reader = SourceReader(srcbuf);
    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, maxSize);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("foob"));

    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, maxSize);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("arba"));

    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, 1);
    EXPECT_EQ(std::string_view(buf, read), std::string_view("z"));

    read = reader.readChunk(buf, maxSize);
    EXPECT_EQ(read, 0);
}
