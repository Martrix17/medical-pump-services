#include "pump_helper.hpp"

#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "device/common/binary_reader.hpp"

std::string helper::pumpA::makePumpAFrame(std::string_view rawMessage) {
    nlohmann::json message = nlohmann::json::parse(rawMessage);
    return message.dump() + "\n";
}

std::string helper::pumpB::makeValidPumpBFrame() {
    std::string frame;
    frame.reserve(PumpBFrameSize);

    // Header
    frame.push_back(static_cast<char>(0xAA));
    frame.push_back(static_cast<char>(0x55));

    // Message type: measurement
    frame.push_back(static_cast<char>(0x02));

    // Status: connected
    frame.push_back(static_cast<char>(0x02));

    // Device ID: 0x00000001
    binary::appendUint32(frame, 1234);

    // Timestamp
    binary::appendUint64(frame, 1758499200);

    // Flow rate = 1.5f
    binary::appendFloat32(frame, 1.5F);

    // Pressure = 2.1f
    binary::appendFloat32(frame, 2.1F);

    // Alarm type = none
    frame.push_back(0x00);

    // Severity = none
    frame.push_back(0x00);

    const auto checksum = binary::calculateChecksum(frame);

    binary::appendUint16(frame, checksum);

    EXPECT_EQ(frame.size(), PumpBFrameSize);

    return frame;
}