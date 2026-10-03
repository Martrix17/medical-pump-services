#include "device/adapter/pump_b_adapter.hpp"

#include <optional>
#include <string>

#include "device/adapter/pump_b_message.hpp"
#include "device/adapter/pump_b_parser.hpp"
#include "device/adapter/pump_b_protocol.hpp"
#include "device/common/timestamp_parser.hpp"
#include "device/domain/device_id.hpp"
#include "device/domain/device_status.hpp"
#include "device/domain/measurement.hpp"
#include "device/domain/state_enums.hpp"

std::vector<DeviceEvent> PumpBAdapter::processMessage(std::string_view rawMessage) {
    latestMessage_ = PumpBParser::parse(rawMessage);

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

DeviceID PumpBAdapter::getDeviceID() const {
    if (!latestMessage_) {
        return DeviceID{""};
    }

    return DeviceID{std::to_string(latestMessage_->deviceID)};
}

std::optional<DeviceStatus> PumpBAdapter::getStatus() const {
    if (!latestMessage_) {
        return std::nullopt;
    }

    const auto& message = *latestMessage_;
    return DeviceStatus{DeviceID{std::to_string(message.deviceID)},
                        device::pumpB::toDomain(message.status),
                        resolveTimestamp(message.timestamp)};
}

std::vector<Alarm> PumpBAdapter::getAlarms() const {
    std::vector<Alarm> alarms;

    if (!latestMessage_ || latestMessage_->type != PumpBMessageType::Alarm ||
        !latestMessage_->alarm) {
        return alarms;
    }

    const auto& message = *latestMessage_;
    alarms.emplace_back(Alarm{DeviceID{std::to_string(message.deviceID)},
                              device::pumpB::toDomain(message.alarm->severity),
                              device::pumpB::toDomain((message.alarm->type)),
                              resolveTimestamp(message.timestamp)});

    return alarms;
}

std::optional<Measurement> PumpBAdapter::getMeasurement() const {
    if (!latestMessage_ || latestMessage_->type != PumpBMessageType::Measurement ||
        !latestMessage_->measurement) {
        return std::nullopt;
    }

    const auto& message = *latestMessage_;
    return Measurement{DeviceID{std::to_string(message.deviceID)}, message.measurement->flowRate,
                       message.measurement->pressure, resolveTimestamp(message.timestamp)};
}

Timestamp PumpBAdapter::resolveTimestamp(std::uint64_t rawTimestamp) {
    return Timestamp{
        std::chrono::duration_cast<Timestamp::duration>(std::chrono::milliseconds{rawTimestamp})};
}
