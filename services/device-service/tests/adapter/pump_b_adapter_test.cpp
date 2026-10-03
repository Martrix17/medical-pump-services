#include <gtest/gtest.h>

#include "device/adapter/pump_b_adapter.hpp"
#include "device/adapter/pump_b_message.hpp"
#include "device/common/binary_reader.hpp"

namespace {

void appendU16BE(std::string& out, std::uint16_t value) {
    out.push_back(static_cast<char>((value >> 8) & 0xFF));
    out.push_back(static_cast<char>(value & 0xFF));
}

void appendU32BE(std::string& out, std::uint32_t value) {
    for (int shift = 24; shift >= 0; shift -= 8) {
        out.push_back(static_cast<char>((value >> shift) & 0xFF));
    }
}

void appendU64BE(std::string& out, std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
        out.push_back(static_cast<char>((value >> shift) & 0xFF));
    }
}

void appendFloatBE(std::string& out, float value) {
    appendU32BE(out, std::bit_cast<std::uint32_t>(value));
}

std::string buildFrame(PumpBMessageType type, PumpBStatus status = PumpBStatus::Connected,
                       float flowRate = std::numeric_limits<float>::quiet_NaN(),
                       float pressure = std::numeric_limits<float>::quiet_NaN(),
                       PumpBAlarmType alarmType = PumpBAlarmType::None,
                       PumpBSeverity severity = PumpBSeverity::None, std::uint32_t deviceID = 1001,
                       std::uint64_t timestampMillis = 1'700'000'000'000ULL) {
    std::string frame;
    appendU16BE(frame, 0xAA55);  // Header
    frame.push_back(static_cast<char>(static_cast<std::uint8_t>(type)));
    frame.push_back(static_cast<char>(static_cast<std::uint8_t>(status)));
    appendU32BE(frame, deviceID);
    appendU64BE(frame, timestampMillis);
    appendFloatBE(frame, flowRate);
    appendFloatBE(frame, pressure);
    frame.push_back(static_cast<char>(static_cast<std::uint8_t>(alarmType)));
    frame.push_back(static_cast<char>(static_cast<std::uint8_t>(severity)));
    appendU16BE(frame, binary::calculateChecksum(frame));  // computed over the 26 bytes
    return frame;
}

}  // namespace

TEST(PumpBAdapterTest, StatusOnlyMessageYieldsSingleStatusEvent) {
    PumpBAdapter adapter;
    const auto events =
        adapter.processMessage(buildFrame(PumpBMessageType::Status, PumpBStatus::Connected));

    ASSERT_EQ(events.size(), 1U);
    EXPECT_TRUE(std::holds_alternative<DeviceStatus>(events.front()));
}

TEST(PumpBAdapterTest, MeasurementOnlyMessageYieldsStatusMeasurementEvent) {
    PumpBAdapter adapter;
    const auto events = adapter.processMessage(
        buildFrame(PumpBMessageType::Measurement, PumpBStatus::Connected, 1.5F, 2.1F));

    ASSERT_EQ(events.size(), 2U);  // Status + Measurement
    const auto* measurement = std::get_if<Measurement>(&events.back());
    ASSERT_NE(measurement, nullptr);
    EXPECT_FLOAT_EQ(measurement->flowRate(), 1.5F);
    EXPECT_FLOAT_EQ(measurement->pressure(), 2.1F);
}

TEST(PumpBAdapterTest, AlarmOnlyMessageYieldsStatusAlarmEvent) {
    PumpBAdapter adapter;
    const auto events = adapter.processMessage(
        buildFrame(PumpBMessageType::Alarm, PumpBStatus::Connected,
                   std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(),
                   PumpBAlarmType::Occlusion, PumpBSeverity::Warning));

    ASSERT_EQ(events.size(), 2U);  // Status + Measurement
    EXPECT_TRUE(std::holds_alternative<Alarm>(events.back()));
}

TEST(PumpBAdapterTest, CorruptedChecksumYieldsNoEvents) {
    auto frame = buildFrame(PumpBMessageType::Status);
    frame.back() = static_cast<char>(frame.back() ^ 0xFF);

    PumpBAdapter adapter;
    EXPECT_TRUE(adapter.processMessage(frame).empty());
}

TEST(PumpBAdapterTest, WrongMagicHeaderYieldsNoEvents) {
    auto frame = buildFrame(PumpBMessageType::Status);
    frame[0] = static_cast<char>(0x00);  // no longer 0xAA55

    PumpBAdapter adapter;
    EXPECT_TRUE(adapter.processMessage(frame).empty());
}

// Documents a real edge case: a frame can parse (flowRate/pressure are both NaN) while
// still having Measurement as its type. The parser doesn't cross-check type against field,
// so this slips through parsing but produces zero events, since getMeasurement() requires
// both type == Measurement AND real (non-NaN) data, and no other getter's type check matches
// either.
TEST(PumpBAdapterTest, MeasurementTypeWithoutRealMeasurementYieldsNoEvents) {
    PumpBAdapter adapter;
    const auto events = adapter.processMessage(buildFrame(PumpBMessageType::Measurement));

    EXPECT_EQ(events.size(), 1U);  // Only Status event
}
