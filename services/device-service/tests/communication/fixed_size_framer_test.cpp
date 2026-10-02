#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "device/communication/fixed_size_framer.hpp"

TEST(FixedSizeFramerTest, ReturnsCompleteFrame) {
    FixedSizeFramer framer(4);

    const auto frames = framer.process("ABCD");

    ASSERT_EQ(frames.size(), 1);
    EXPECT_EQ(frames[0], "ABCD");
}

TEST(FixedSizeFramerTest, BuffersIncompleteFrame) {
    FixedSizeFramer framer(4);

    const auto frames = framer.process("ABC");

    EXPECT_TRUE(frames.empty());
}

TEST(FixedSizeFramerTest, CompletesFrameAcrossMultipleCalls) {
    FixedSizeFramer framer(4);

    auto frames = framer.process("AB");

    EXPECT_TRUE(frames.empty());

    frames = framer.process("CD");

    ASSERT_EQ(frames.size(), 1);
    EXPECT_EQ(frames[0], "ABCD");
}

TEST(FixedSizeFramerTest, ReturnsMultipleFrames) {
    FixedSizeFramer framer(4);

    const auto frames = framer.process("ABCDEFGH");

    ASSERT_EQ(frames.size(), 2);
    EXPECT_EQ(frames[0], "ABCD");
    EXPECT_EQ(frames[1], "EFGH");
}

TEST(FixedSizeFramerTest, ReturnsMultipleFramesAndBuffersPartialFrame) {
    FixedSizeFramer framer(4);

    const auto frames = framer.process("ABCDEFGHIJ");

    ASSERT_EQ(frames.size(), 2);
    EXPECT_EQ(frames[0], "ABCD");
    EXPECT_EQ(frames[1], "EFGH");
}

TEST(FixedSizeFramerTest, CompletesPartialFrameOnNextCall) {
    FixedSizeFramer framer(4);

    auto frames = framer.process("ABCDEFGHIJ");

    ASSERT_EQ(frames.size(), 2);

    frames = framer.process("KL");

    ASSERT_EQ(frames.size(), 1);
    EXPECT_EQ(frames[0], "IJKL");
}

TEST(FixedSizeFramerTest, ResetDiscardsBufferedData) {
    FixedSizeFramer framer(4);

    auto frames = framer.process("ABC");

    EXPECT_TRUE(frames.empty());

    framer.reset();

    frames = framer.process("DEFG");

    ASSERT_EQ(frames.size(), 1);
    EXPECT_EQ(frames[0], "DEFG");
}

TEST(FixedSizeFramerTest, RejectsZeroFrameSize) {
    EXPECT_THROW(FixedSizeFramer(0), std::invalid_argument);
}

TEST(FixedSizeFramerTest, HandlesPumpBFrameSize) {
    FixedSizeFramer framer(28);

    const std::string firstPart(10, 'A');
    const std::string secondPart(18, 'B');

    auto frames = framer.process(firstPart);
    EXPECT_TRUE(frames.empty());

    frames = framer.process(secondPart);

    ASSERT_EQ(frames.size(), 1);
    EXPECT_EQ(frames[0].size(), 28);
}
