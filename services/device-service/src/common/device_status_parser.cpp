#include "device/common/device_status_parser.hpp"

#include <unordered_map>

State device::common::parseState(std::string_view status) {
    static const std::unordered_map<std::string_view, State> table{
        {"Disconnected", State::Disconnected}, {"Connecting", State::Connecting},
        {"Connected", State::Connected},       {"Error", State::Error},
        {"Unknown", State::Unknown},
    };

    if (const auto it = table.find(status); it != table.end()) {
        return it->second;
    }

    return State::Unknown;
}