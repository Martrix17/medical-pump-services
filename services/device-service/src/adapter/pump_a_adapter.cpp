#include "device/adapter/pump_a_adapter.hpp"

#include "device/adapter/pump_a_parser.hpp"
#include "device/common/alarm_parser.hpp"
#include "device/common/device_status_parser.hpp"
#include "device/common/timestamp_parser.hpp"
#include "device/domain/device_id.hpp"

bool PumpAAdapter::processMessage(std::string_view rawMessage) {
    latestMessage_ = PumpAParser::parse(rawMessage);
    return static_cast<bool>(latestMessage_);
}

DeviceID PumpAAdapter::getDeviceID() {
    if (!latestMessage_) {
        return DeviceID{""};
    }

    const auto& message = *latestMessage_;

    return message.deviceID;
}

DeviceStatus PumpAAdapter::getStatus() {
    if (!latestMessage_) {
        return DeviceStatus::Unknown;
    }

    return device::common::parseDeviceStatus(latestMessage_->status);
}

std::vector<Alarm> PumpAAdapter::getAlarms() {
    std::vector<Alarm> alarms;

    if (!latestMessage_ || !latestMessage_->alarm) {
        return alarms;
    }

    const auto& message = *latestMessage_;

    auto timestamp = device::common::parseTimestamp(message.timestamp);
    if (!timestamp) {
        timestamp = Timestamp::clock::now();
    }

    alarms.emplace_back(Alarm{message.deviceID,
                              device::common::parseSeverity(message.alarm->severity),
                              device::common::parseAlarmType(message.alarm->type), *timestamp});

    return alarms;
}

std::optional<Measurement> PumpAAdapter::getMeasurement() {
    if (!latestMessage_) {
        return std::nullopt;
    }

    const auto& message = *latestMessage_;

    auto timestamp = device::common::parseTimestamp(message.timestamp);
    if (!timestamp) {
        timestamp = Timestamp::clock::now();
    }

    return Measurement{message.deviceID, message.flowRate, message.pressure, *timestamp};
}
