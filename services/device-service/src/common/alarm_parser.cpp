#include "device/common/alarm_parser.hpp"

#include <unordered_map>

Severity device::common::parseSeverity(std::string_view severity) {
    static const std::unordered_map<std::string_view, Severity> table{
        {"Info", Severity::Info},
        {"Warning", Severity::Warning},
        {"Critical", Severity::Critical},
        {"Unknown", Severity::Unknown},
    };

    if (const auto it = table.find(severity); it != table.end()) {
        return it->second;
    }

    return Severity::Unknown;
}

AlarmType device::common::parseAlarmType(std::string_view type) {
    static const std::unordered_map<std::string_view, AlarmType> table{
        {"Occlusion", AlarmType::Occlusion},
        {"Low_Pressure", AlarmType::LowPressure},
        {"Device_Failure", AlarmType::DeviceFailure},
        {"Unknown", AlarmType::Unknown},
    };

    if (const auto it = table.find(type); it != table.end()) {
        return it->second;
    }

    return AlarmType::Unknown;
}