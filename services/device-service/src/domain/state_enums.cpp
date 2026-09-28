#include "device/domain/state_enums.hpp"

std::string_view toString(State status) {
    switch (status) {
        case State::Disconnected:
            return "Disconnected";
        case State::Connecting:
            return "Connecting";
        case State::Connected:
            return "Connected";
        case State::Error:
            return "Error";
        case State::Unknown:
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
