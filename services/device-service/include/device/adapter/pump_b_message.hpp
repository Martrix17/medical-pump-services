#ifndef PUMP_B_MESSAGE_HPP
#define PUMP_B_MESSAGE_HPP

#include <cstdint>
#include <optional>
#include <string>

enum class PumpBMessageType : std::uint8_t {
    Status = 0x01,
    Measurement = 0x02,
    Alarm = 0x03,
};

enum class PumpBStatus : std::uint8_t {
    Disconnected = 0x00,
    Connecting = 0x01,
    Connected = 0x02,
    Error = 0x03,
    Unknown = 0xFF,
};

enum class PumpBAlarmType : std::uint8_t {
    None = 0x00,
    Occlusion = 0x01,
    LowPressure = 0x02,
    DeviceFailure = 0x03,
    Unknown = 0x04,
};

enum class PumpBSeverity : std::uint8_t {
    None = 0x00,
    Info = 0x01,
    Warning = 0x02,
    Critical = 0x03,
};

struct PumpBMeasurement {
    float flowRate;
    float pressure;
};

struct PumpBAlarm {
    PumpBAlarmType type;
    PumpBSeverity severity;
};

struct PumpBMessage {
    PumpBMessageType type;
    std::uint32_t deviceID;
    std::uint64_t timestamp;
    PumpBStatus status;
    std::optional<PumpBMeasurement> measurement;
    std::optional<PumpBAlarm> alarm;
};

#endif  // PUMP_B_MESSAGE_HPP
