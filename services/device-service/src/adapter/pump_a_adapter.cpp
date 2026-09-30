#include "device/adapter/pump_a_adapter.hpp"

#include <optional>

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
    if (auto status = getStatus()) {
        events.emplace_back(std::move(*status));
    }

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

    return DeviceID{latestMessage_->deviceID};
}

std::optional<DeviceStatus> PumpAAdapter::getStatus() const {
    if (!latestMessage_) {
        return std::nullopt;
    }

    const auto& message = *latestMessage_;
    return DeviceStatus{DeviceID{message.deviceID},
                        device::common::parseDeviceState(latestMessage_->status),
                        resolveTimestamp(message.timestamp)};
}

std::vector<Alarm> PumpAAdapter::getAlarms() const {
    std::vector<Alarm> alarms;

    if (!latestMessage_ || !latestMessage_->alarm) {
        return alarms;
    }

    const auto& message = *latestMessage_;
    alarms.emplace_back(Alarm{
        DeviceID{message.deviceID}, device::common::parseSeverity(message.alarm->severity),
        device::common::parseAlarmType(message.alarm->type), resolveTimestamp(message.timestamp)});

    return alarms;
}

std::optional<Measurement> PumpAAdapter::getMeasurement() const {
    if (!latestMessage_ || !latestMessage_->measurement) {
        return std::nullopt;
    }

    const auto& message = *latestMessage_;
    return Measurement{DeviceID{message.deviceID}, message.measurement->flowRate,
                       message.measurement->pressure, resolveTimestamp(message.timestamp)};
}

Timestamp PumpAAdapter::resolveTimestamp(const std::string& rawTimestamp) {
    if (auto timestamp = device::common::parseTimestamp(rawTimestamp)) {
        return *timestamp;
    }

    return Timestamp::clock::now();
}
