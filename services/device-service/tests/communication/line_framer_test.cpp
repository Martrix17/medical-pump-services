#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "device/communication/line_framer.hpp"

TEST(LineFramerTest, ReturnsCompleteMessage) {
    LineFramer framer;

    const auto messages = framer.process("hello\n");

    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0], "hello");
}

TEST(LineFramerTest, ReturnsMultipleMessages) {
    LineFramer framer;

    const auto messages = framer.process("hello\nworld\n");

    ASSERT_EQ(messages.size(), 2);
    EXPECT_EQ(messages[0], "hello");
    EXPECT_EQ(messages[1], "world");
}

TEST(LineFramerTest, BuffersIncompleteMessage) {
    LineFramer framer;

    const auto messages = framer.process("hello");

    EXPECT_TRUE(messages.empty());
}

TEST(LineFramerTest, CompletesMessageAcrossMultipleCalls) {
    LineFramer framer;

    auto messages = framer.process("hel");
    EXPECT_TRUE(messages.empty());

    messages = framer.process("lo\n");

    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0], "hello");
}

TEST(LineFramerTest, HandlesMultipleMessagesAcrossCalls) {
    LineFramer framer;

    auto messages = framer.process("hello\nwor");

    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0], "hello");

    messages = framer.process("ld\n");

    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0], "world");
}

TEST(LineFramerTest, HandlesEmptyMessage) {
    LineFramer framer;

    const auto messages = framer.process("\n");

    ASSERT_EQ(messages.size(), 1);
    EXPECT_TRUE(messages[0].empty());
}

TEST(LineFramerTest, ResetDiscardsBufferedData) {
    LineFramer framer;

    auto messages = framer.process("incomplete");
    EXPECT_TRUE(messages.empty());

    framer.reset();

    messages = framer.process("message\n");

    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0], "message");
}
