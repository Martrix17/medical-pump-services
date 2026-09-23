#ifndef STATUS_HPP
#define STATUS_HPP

enum class DeviceStatus { Disconnected, Connecting, Connected, Error, Unknown };

enum class Severity { Info, Warning, Critical, Unknown };

enum class AlarmType { Occlusion, LowPressure, DeviceFailure, Unknown };

#endif  // STATUS_HPP