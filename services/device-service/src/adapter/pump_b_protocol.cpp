#include "device/adapter/pump_b_protocol.hpp"

std::optional<PumpBMessageType> device::pumpB::parseMessageType(std::uint8_t value) {
    switch (value) {
        case 0x01:
            return PumpBMessageType::Status;
        case 0x02:
            return PumpBMessageType::Measurement;
        case 0x03:
            return PumpBMessageType::Alarm;
        default:
            return std::nullopt;
    }
}

std::optional<PumpBStatus> device::pumpB::parseStatus(std::uint8_t value) {
    switch (value) {
        case 0x00:
            return PumpBStatus::Disconnected;
        case 0x01:
            return PumpBStatus::Connecting;
        case 0x02:
            return PumpBStatus::Connected;
        case 0x03:
            return PumpBStatus::Error;
        case 0xFF:
            return PumpBStatus::Unknown;
        default:
            return std::nullopt;
    }
}

std::optional<PumpBAlarmType> device::pumpB::parseAlarmType(std::uint8_t value) {
    switch (value) {
        case 0x01:
            return PumpBAlarmType::Occlusion;
        case 0x02:
            return PumpBAlarmType::LowPressure;
        case 0x03:
            return PumpBAlarmType::DeviceFailure;
        default:
            return std::nullopt;
    }
}

std::optional<PumpBSeverity> device::pumpB::parseSeverity(std::uint8_t value) {
    switch (value) {
        case 0x01:
            return PumpBSeverity::Info;
        case 0x02:
            return PumpBSeverity::Warning;
        case 0x03:
            return PumpBSeverity::Critical;
        default:
            return std::nullopt;
    }
}

DeviceState device::pumpB::toDomain(PumpBStatus status) {
    switch (status) {
        case PumpBStatus::Disconnected:
            return DeviceState::Disconnected;
        case PumpBStatus::Connecting:
            return DeviceState::Connecting;
        case PumpBStatus::Connected:
            return DeviceState::Connected;
        case PumpBStatus::Error:
            return DeviceState::Error;
        case PumpBStatus::Unknown:
            return DeviceState::Unknown;
    }
    return DeviceState::Unknown;
}

AlarmType device::pumpB::toDomain(PumpBAlarmType type) {
    switch (type) {
        case PumpBAlarmType::None:
            return AlarmType::Unknown;
        case PumpBAlarmType::Occlusion:
            return AlarmType::Occlusion;
        case PumpBAlarmType::LowPressure:
            return AlarmType::LowPressure;
        case PumpBAlarmType::DeviceFailure:
            return AlarmType::DeviceFailure;
        case PumpBAlarmType::Unknown:
            return AlarmType::Unknown;
    }
    return AlarmType::Unknown;
}

Severity device::pumpB::toDomain(PumpBSeverity severity) {
    switch (severity) {
        case PumpBSeverity::None:
            return Severity::Unknown;
        case PumpBSeverity::Info:
            return Severity::Info;
        case PumpBSeverity::Warning:
            return Severity::Warning;
        case PumpBSeverity::Critical:
            return Severity::Critical;
    }
    return Severity::Unknown;
}