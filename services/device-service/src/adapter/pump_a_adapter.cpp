#include "device/adapter/pump_a_adapter.hpp"

#include "device/adapter/pump_a_parser.hpp"
#include "device/common/alarm_parser.hpp"
#include "device/common/device_status_parser.hpp"
#include "device/common/timestamp_parser.hpp"
#include "device/domain/device_id.hpp"
#include "device/domain/device_status.hpp"
#include "device/domain/measurement.hpp"
#include "device/domain/state_enums.hpp"

std::vector<DeviceEvent> PumpAAdapter::processMessage(std::string_view rawMessage) {
    latestMessage_ = PumpAParser::parse(rawMessage);

    if (!latestMessage_) {
        return {};
    }

    std::vector<DeviceEvent> events;
    events.emplace_back(getStatus());

    if (auto measurement = getMeasurement()) {
        events.emplace_back(std::move(*measurement));
    }

    for (auto& alarm : getAlarms()) {
        events.emplace_back(std::move(alarm));
    }

    return events;
}

DeviceID PumpAAdapter::getDeviceID() const {
    if (!latestMessage_) {
        return DeviceID{""};
    }

    const auto& message = *latestMessage_;

    return DeviceID{message.deviceID};
}

DeviceStatus PumpAAdapter::getStatus() const {
    if (!latestMessage_) {
        return DeviceStatus{getDeviceID(), DeviceState::Unknown, Timestamp::clock::now()};
    }

    const auto& message = *latestMessage_;
    const auto state = device::common::parseDeviceState(latestMessage_->status);
    const auto timestamp = resolveTimestamp(message.timestamp);

    return DeviceStatus{DeviceID{message.deviceID}, state, timestamp};
}

std::vector<Alarm> PumpAAdapter::getAlarms() const {
    std::vector<Alarm> alarms;

    if (!latestMessage_ || !latestMessage_->alarm) {
        return alarms;
    }

    const auto& message = *latestMessage_;
    const auto timestamp = resolveTimestamp(message.timestamp);

    alarms.emplace_back(Alarm{DeviceID{message.deviceID},
                              device::common::parseSeverity(message.alarm->severity),
                              device::common::parseAlarmType(message.alarm->type), timestamp});

    return alarms;
}

std::optional<Measurement> PumpAAdapter::getMeasurement() const {
    if (!latestMessage_) {
        return std::nullopt;
    }

    const auto& message = *latestMessage_;
    const auto timestamp = resolveTimestamp(message.timestamp);

    return Measurement{DeviceID{message.deviceID}, message.flowRate, message.pressure, timestamp};
}

Timestamp PumpAAdapter::resolveTimestamp(const std::string& rawTimestamp) const {
    if (auto timestamp = device::common::parseTimestamp(rawTimestamp)) {
        return *timestamp;
    }

    return Timestamp::clock::now();
}
