#include <gtest/gtest.h>
#include <string>

#include "APPS/FXPLOT/table_text_buffer.hh"

class TableTextBufferTest : public testing::Test {
protected:
    char *buffer = NULL;
    size_t capacity = 0;
    size_t position = 0;

    void TearDown() {
        free(buffer);
    }
};

TEST_F(TableTextBufferTest, AppendsFormattedText) {
    buffer = twprint(buffer, &capacity, &position, (char *)"%s: %d", "value", 42);
    buffer = twprint(buffer, &capacity, &position, (char *)"%s", " done");
    EXPECT_STREQ("value: 42 done", buffer);
    EXPECT_EQ(strlen(buffer), position);
    EXPECT_GT(capacity, position);
}

TEST_F(TableTextBufferTest, PreservesLongMessages) {
    std::string input(16384, 'x');
    buffer = twprint(buffer, &capacity, &position, (char *)"%s:%d", input.c_str(), 42);
    EXPECT_EQ(input + ":42", buffer);
    EXPECT_EQ(input.size() + 3, position);
    EXPECT_GT(capacity, position);
}

TEST_F(TableTextBufferTest, HandlesLargeFormatWidths) {
    buffer = twprint(buffer, &capacity, &position, (char *)"%10000d", 1);
    EXPECT_EQ(std::string(9999, ' ') + "1", buffer);
    EXPECT_EQ(10000u, position);
}

TEST_F(TableTextBufferTest, GrowsForTheNullTerminator) {
    capacity = 8;
    position = 7;
    buffer = static_cast<char *>(calloc(capacity, 1));
    strcpy(buffer, "1234567");
    buffer = twprint(buffer, &capacity, &position, (char *)"%s", "8");
    EXPECT_STREQ("12345678", buffer);
    EXPECT_EQ(8u, position);
    EXPECT_GT(capacity, position);
}

TEST_F(TableTextBufferTest, InitializesAnEmptyBuffer) {
    buffer = twprint(buffer, &capacity, &position, (char *)"%s", "");
    ASSERT_NE(nullptr, buffer);
    EXPECT_STREQ("", buffer);
    EXPECT_EQ(0u, position);
    EXPECT_GT(capacity, position);
}

TEST_F(TableTextBufferTest, PreservesContentsAcrossRepeatedGrowth) {
    std::string expected;
    for (int i = 0; i < 40; ++i) {
        std::string chunk(2000, 'a' + i % 26);
        expected += chunk;
        buffer = twprint(buffer, &capacity, &position, (char *)"%s", chunk.c_str());
        EXPECT_EQ(expected, buffer);
        EXPECT_EQ(expected.size(), position);
        EXPECT_GT(capacity, position);
    }
}

TEST_F(TableTextBufferTest, RejectsAnUnrepresentableBufferSize) {
    position = std::numeric_limits<size_t>::max();
    EXPECT_EXIT(twprint(buffer, &capacity, &position, "%s", "x"),
                testing::ExitedWithCode(1), "OUT_OF_MEMORY in twprint");
}
