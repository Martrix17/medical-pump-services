#ifndef STATUS_HPP
#define STATUS_HPP

enum class DeviceState {
    Disconnected,
    Connecting,
    Connected,
    Error
};

enum class Severity {
    Info,
    Warning,
    Critical
};

enum class AlarmType
{
    Occlusion,
    LowPressure,
    DeviceFailure
};


#endif // STATUS_HPP