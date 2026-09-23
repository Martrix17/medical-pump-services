#include "device/domain/alarm.hpp"

Alarm::Alarm(DeviceID deviceID, Severity severity, AlarmType type, Timestamp timestamp)
    : deviceID_(std::move(deviceID)), severity_(severity), type_(type), timestamp_(timestamp) {}

const DeviceID& Alarm::deviceID() const noexcept {
    return deviceID_;
}

Severity Alarm::severity() const noexcept {
    return severity_;
}

AlarmType Alarm::type() const noexcept {
    return type_;
}

Timestamp Alarm::timestamp() const noexcept {
    return timestamp_;
}