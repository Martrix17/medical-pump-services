#include "device/common/device_status_parser.hpp"

#include <unordered_map>

DeviceState device::common::parseDeviceState(std::string_view status) {
    static const std::unordered_map<std::string_view, DeviceState> table{
        {"Disconnected", DeviceState::Disconnected}, {"Connecting", DeviceState::Connecting},
        {"Connected", DeviceState::Connected},       {"Error", DeviceState::Error},
        {"Unknown", DeviceState::Unknown},
    };

    if (const auto it = table.find(status); it != table.end()) {
        return it->second;
    }

    return DeviceState::Unknown;
}