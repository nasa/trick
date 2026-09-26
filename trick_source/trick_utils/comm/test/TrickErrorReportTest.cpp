#include <gtest/gtest.h>
#include <string>

#include "trick/trick_error_hndlr.h"

class TrickErrorReportTest : public testing::Test {
protected:
    TrickErrorHndlr handler;
    std::string message;
    TrickErrorLevel level;
    int calls;

    static void capture(TrickErrorHndlr *handler, TrickErrorLevel level,
                        const char *, int, const char *message) {
        TrickErrorReportTest *test = static_cast<TrickErrorReportTest *>(handler->data_ptr);
        test->message = message;
        test->level = level;
        ++test->calls;
    }

    void SetUp() {
        calls = 0;
        trick_error_init(&handler, capture, this, TRICK_ERROR_ALL);
    }
};

TEST_F(TrickErrorReportTest, PreservesShortFormattedMessages) {
    trick_error_report(&handler, TRICK_ERROR_WARNING, __FILE__, __LINE__,
                       "device %s returned %d", "alpha", 42);
    EXPECT_EQ("device alpha returned 42", message);
    EXPECT_EQ(TRICK_ERROR_WARNING, level);
    EXPECT_EQ(1, calls);
}

TEST_F(TrickErrorReportTest, TerminatesMessagesAtTheBufferBoundary) {
    for (size_t size = 4094; size <= 4097; ++size) {
        std::string input(size, 'x');
        trick_error_report(&handler, TRICK_ERROR_WARNING, __FILE__, __LINE__, "%s", input.c_str());
        EXPECT_EQ(input.substr(0, 4095), message);
    }
}

TEST_F(TrickErrorReportTest, TruncatesOversizedVariadicMessages) {
    std::string input(16384, 'x');
    trick_error_report(&handler, TRICK_ERROR_WARNING, __FILE__, __LINE__,
                       "prefix:%s:%d", input.c_str(), 42);
    EXPECT_EQ(("prefix:" + input).substr(0, 4095), message);
    EXPECT_EQ(1, calls);
}

TEST_F(TrickErrorReportTest, BoundsFormatWidthExpansion) {
    trick_error_report(&handler, TRICK_ERROR_WARNING, __FILE__, __LINE__, "%10000d", 1);
    EXPECT_EQ(std::string(4095, ' '), message);
}

TEST_F(TrickErrorReportTest, SuppressesMessagesBelowReportLevel) {
    handler.report_level = TRICK_ERROR_ALERT;
    std::string input(16384, 'x');
    trick_error_report(&handler, TRICK_ERROR_ADVISORY, __FILE__, __LINE__, "%s", input.c_str());
    EXPECT_EQ(0, calls);
}

TEST_F(TrickErrorReportTest, PreservesEmptyFormatDiagnostic) {
    trick_error_report(&handler, TRICK_ERROR_WARNING, __FILE__, __LINE__, "%s", "");
    EXPECT_EQ("", message);
    trick_error_report(&handler, TRICK_ERROR_WARNING, __FILE__, __LINE__, "");
    EXPECT_EQ("Empty message for trick_error_report.\n", message);
    EXPECT_EQ(TRICK_ERROR_ALERT, level);
}
