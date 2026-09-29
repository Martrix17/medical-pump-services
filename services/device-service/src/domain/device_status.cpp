#include "device/domain/device_status.hpp"

#include "device/domain/device_id.hpp"

DeviceStatus::DeviceStatus(DeviceID deviceID, DeviceState deviceState, Timestamp timestamp)
    : deviceID_(std::move(deviceID)), deviceState_(deviceState), timestamp_(timestamp) {}

const DeviceID& DeviceStatus::deviceID() const noexcept {
    return deviceID_;
}

DeviceState DeviceStatus::deviceState() const noexcept {
    return deviceState_;
}

Timestamp DeviceStatus::timestamp() const noexcept {
    return timestamp_;
}