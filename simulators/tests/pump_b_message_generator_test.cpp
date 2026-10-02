#include <gtest/gtest.h>

#include <array>
#include <bit>
#include <cstdint>
#include <ranges>
#include <string_view>

#include "pump_b_message_generator.hpp"

namespace {

constexpr std::size_t kFrameSize = 28;

std::uint8_t byteAt(std::string_view message, std::size_t offset) {
    return static_cast<std::uint8_t>(static_cast<unsigned char>(message.at(offset)));
}

std::uint16_t readUint16BE(std::string_view message, std::size_t offset) {
    return (static_cast<std::uint16_t>(byteAt(message, offset)) << 8) |
           static_cast<std::uint16_t>(byteAt(message, offset + 1));
}

std::uint32_t readUint32BE(std::string_view message, std::size_t offset) {
    return (static_cast<std::uint32_t>(byteAt(message, offset)) << 24) |
           (static_cast<std::uint32_t>(byteAt(message, offset + 1)) << 16) |
           (static_cast<std::uint32_t>(byteAt(message, offset + 2)) << 8) |
           static_cast<std::uint32_t>(byteAt(message, offset + 3));
}

std::uint64_t readUint64BE(std::string_view message, std::size_t offset) {
    std::uint64_t value = 0;

    for (std::size_t i = 0; i < sizeof(std::uint64_t); ++i) {
        value = (value << 8) | byteAt(message, offset + i);
    }

    return value;
}

std::uint16_t calculateChecksum(std::string_view data) {
    std::uint16_t checksum = 0;

    for (const unsigned char byte : data) {
        checksum += byte;
    }

    return checksum;
}

TEST(PumpBMessageGeneratorTest, GeneratesValidFrame) {
    PumpBMessageGenerator generator{1};

    const auto message = generator.generate();

    ASSERT_FALSE(message.empty());
    EXPECT_EQ(message.size(), kFrameSize);
}

TEST(PumpBMessageGeneratorTest, ContainsValidHeader) {
    PumpBMessageGenerator generator{1};

    const auto message = generator.generate();

    ASSERT_EQ(message.size(), kFrameSize);

    EXPECT_EQ(byteAt(message, 0), 0xAA);
    EXPECT_EQ(byteAt(message, 1), 0x55);
}

TEST(PumpBMessageGeneratorTest, UsesConfiguredDeviceId) {
    constexpr std::uint32_t expectedDeviceId = 123456789;

    PumpBMessageGenerator generator{expectedDeviceId};

    const auto message = generator.generate();

    ASSERT_EQ(message.size(), kFrameSize);

    EXPECT_EQ(readUint32BE(message, 4), expectedDeviceId);
}

TEST(PumpBMessageGeneratorTest, GeneratesValidMessageType) {
    constexpr std::array validTypes{
        std::uint8_t{0x01},
        std::uint8_t{0x02},
        std::uint8_t{0x03},
    };

    PumpBMessageGenerator generator{1};

    for (int i = 0; i < 100; ++i) {
        const auto message = generator.generate();

        ASSERT_EQ(message.size(), kFrameSize);

        const auto type = byteAt(message, 2);

        EXPECT_TRUE(std::ranges::find(validTypes, type) != validTypes.end());
    }
}

TEST(PumpBMessageGeneratorTest, GeneratesValidStatus) {
    constexpr std::array validStatuses{
        std::uint8_t{0x00}, std::uint8_t{0x01}, std::uint8_t{0x02},
        std::uint8_t{0x03}, std::uint8_t{0xFF},
    };

    PumpBMessageGenerator generator{1};

    for (int i = 0; i < 100; ++i) {
        const auto message = generator.generate();

        ASSERT_EQ(message.size(), kFrameSize);

        const auto status = byteAt(message, 3);

        EXPECT_TRUE(std::ranges::find(validStatuses, status) != validStatuses.end());
    }
}

TEST(PumpBMessageGeneratorTest, GeneratesValidTimestamp) {
    PumpBMessageGenerator generator{1};

    for (int i = 0; i < 100; ++i) {
        const auto message = generator.generate();

        ASSERT_EQ(message.size(), kFrameSize);

        const auto timestamp = readUint64BE(message, 8);

        EXPECT_GT(timestamp, 0);
    }
}

TEST(PumpBMessageGeneratorTest, GeneratesValidMeasurementValues) {
    PumpBMessageGenerator generator{1};

    int nanCount = 0;
    for (int i = 0; i < 100; ++i) {
        const auto message = generator.generate();

        ASSERT_EQ(message.size(), kFrameSize);

        const auto flowBits = readUint32BE(message, 16);
        const auto pressureBits = readUint32BE(message, 20);

        const auto flowRate = std::bit_cast<float>(flowBits);
        const auto pressure = std::bit_cast<float>(pressureBits);

        if (std::isnan(flowRate) || std::isnan(pressure)) {
            EXPECT_TRUE(std::isnan(flowRate));
            EXPECT_TRUE(std::isnan(pressure));
            ++nanCount;
            continue;
        }

        EXPECT_GE(flowRate, 0.0F);
        EXPECT_LE(flowRate, 5.0F);

        EXPECT_GE(pressure, 0.0F);
        EXPECT_LE(pressure, 4.0F);
    }

    EXPECT_GT(nanCount, 0);
    EXPECT_LT(nanCount, 100);
}

TEST(PumpBMessageGeneratorTest, GeneratesValidAlarmFields) {
    constexpr std::array validAlarmTypes{
        std::uint8_t{0x00}, std::uint8_t{0x01}, std::uint8_t{0x02},
        std::uint8_t{0x03}, std::uint8_t{0x04},
    };

    constexpr std::array validSeverities{
        std::uint8_t{0x00},
        std::uint8_t{0x01},
        std::uint8_t{0x02},
        std::uint8_t{0x03},
    };

    PumpBMessageGenerator generator{1};

    for (int i = 0; i < 100; ++i) {
        const auto message = generator.generate();

        ASSERT_EQ(message.size(), kFrameSize);

        const auto alarmType = byteAt(message, 24);
        const auto severity = byteAt(message, 25);

        EXPECT_TRUE(std::ranges::find(validAlarmTypes, alarmType) != validAlarmTypes.end());

        EXPECT_TRUE(std::ranges::find(validSeverities, severity) != validSeverities.end());
    }
}

TEST(PumpBMessageGeneratorTest, AlarmAndSeverityAreConsistent) {
    PumpBMessageGenerator generator{1};

    for (int i = 0; i < 100; ++i) {
        const auto message = generator.generate();

        ASSERT_EQ(message.size(), kFrameSize);

        const auto alarmType = byteAt(message, 24);
        const auto severity = byteAt(message, 25);

        if (alarmType == 0x00) {
            EXPECT_EQ(severity, 0x00);
        } else {
            EXPECT_NE(severity, 0x00);
        }
    }
}

TEST(PumpBMessageGeneratorTest, GeneratesValidChecksum) {
    PumpBMessageGenerator generator{1};

    for (int i = 0; i < 100; ++i) {
        const auto message = generator.generate();

        ASSERT_EQ(message.size(), kFrameSize);

        const auto expectedChecksum = calculateChecksum(std::string_view(message).substr(0, 26));

        const auto actualChecksum = readUint16BE(message, 26);

        EXPECT_EQ(actualChecksum, expectedChecksum);
    }
}

}  // namespace
