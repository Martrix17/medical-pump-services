#include "device/common/device_status_parser.hpp"

#include <unordered_map>

DeviceStatus device::common::parseDeviceStatus(std::string_view status) {
    static const std::unordered_map<std::string_view, DeviceStatus> table{
        {"Disconnected", DeviceStatus::Disconnected}, {"Connecting", DeviceStatus::Connecting},
        {"Connected", DeviceStatus::Connected},       {"Error", DeviceStatus::Error},
        {"Unknown", DeviceStatus::Unknown},
    };

    if (const auto it = table.find(status); it != table.end()) {
        return it->second;
    }

    return DeviceStatus::Unknown;
}