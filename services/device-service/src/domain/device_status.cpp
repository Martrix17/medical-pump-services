#include "device/domain/device_status.hpp"

#include "device/domain/device_id.hpp"

DeviceStatus::DeviceStatus(DeviceID deviceID, State state, Timestamp timestamp)
    : deviceID_(std::move(deviceID)), state_(state), timestamp_(timestamp) {}

const DeviceID& DeviceStatus::deviceID() const noexcept {
    return deviceID_;
}

State DeviceStatus::state() const noexcept {
    return state_;
}

Timestamp DeviceStatus::timestamp() const noexcept {
    return timestamp_;
}