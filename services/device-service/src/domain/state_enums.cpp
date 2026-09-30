#include "device/domain/state_enums.hpp"

std::string_view toString(DeviceState state) {
    switch (state) {
        case DeviceState::Disconnected:
            return "Disconnected";
        case DeviceState::Connecting:
            return "Connecting";
        case DeviceState::Connected:
            return "Connected";
        case DeviceState::Error:
            return "Error";
        case DeviceState::Unknown:
            return "Unknown";
    }

    return "Unknown";
}

std::string_view toString(Severity severity) {
    switch (severity) {
        case Severity::Info:
            return "Info";
        case Severity::Warning:
            return "Warning";
        case Severity::Critical:
            return "Critical";
        case Severity::Unknown:
            return "Unknown";
    }

    return "Unknown";
}

std::string_view toString(AlarmType type) {
    switch (type) {
        case AlarmType::Occlusion:
            return "Occlusion";
        case AlarmType::LowPressure:
            return "LowPressure";
        case AlarmType::DeviceFailure:
            return "DeviceFailure";
        case AlarmType::Unknown:
            return "Unknown";
    }

    return "Unknown";
}
