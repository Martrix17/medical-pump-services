#include "device/adapter/pump_b_parser.hpp"

#include <bit>
#include <cmath>

#include "device/adapter/pump_b_message.hpp"
#include "device/adapter/pump_b_protocol.hpp"
#include "device/common/binary_reader.hpp"

std::optional<PumpBMessage> PumpBParser::parse(std::string_view rawMessage) {
    constexpr std::size_t kFrameSize = 28;
    constexpr std::uint16_t kMagicHeader = 0xAA55;
    constexpr std::size_t kChecksumOffset = 26;

    if (rawMessage.size() != kFrameSize) {
        return std::nullopt;
    }

    if (binary::readUint16(rawMessage, 0) != kMagicHeader) {
        return std::nullopt;
    }

    const auto expectedChecksum = binary::calculateChecksum(rawMessage.substr(0, kChecksumOffset));
    if (binary::readUint16(rawMessage, kChecksumOffset) != expectedChecksum) {
        return std::nullopt;
    }

    const auto type = device::pumpB::parseMessageType(static_cast<std::uint8_t>(rawMessage[2]));
    if (!type) {
        return std::nullopt;
    }

    const auto status = device::pumpB::parseStatus(static_cast<std::uint8_t>(rawMessage[3]));
    if (!status) {
        return std::nullopt;
    }

    const auto deviceID = binary::readUint32(rawMessage, 4);
    const auto timestamp = binary::readUint64(rawMessage, 8);

    PumpBMessage message{
        .type = *type, .deviceID = deviceID, .timestamp = timestamp, .status = *status};

    const float flowRate = binary::readFloat32(rawMessage, 16);
    const float pressure = binary::readFloat32(rawMessage, 20);

    const bool hasFlowRate = !std::isnan(flowRate);
    const bool hasPressure = !std::isnan(pressure);

    if (hasFlowRate != hasPressure) {
        return std::nullopt;
    }

    if (hasFlowRate) {
        message.measurement = PumpBMeasurement{flowRate, pressure};
    }

    const auto alarmType = device::pumpB::parseAlarmType(static_cast<std::uint8_t>(rawMessage[24]));
    const auto severity = device::pumpB::parseSeverity(static_cast<std::uint8_t>(rawMessage[25]));

    if (!alarmType || !severity) {
        return std::nullopt;
    }

    const bool hasAlarmType = (*alarmType != PumpBAlarmType::None);
    const bool hasSeverity = (*severity != PumpBSeverity::None);

    if (hasAlarmType != hasSeverity) {
        return std::nullopt;
    }

    if (hasAlarmType) {
        message.alarm = PumpBAlarm{*alarmType, *severity};
    }

    return message;
}
