#ifndef STATUS_HPP
#define STATUS_HPP

#include <string_view>

enum class DeviceState { Disconnected, Connecting, Connected, Error, Unknown };

[[nodiscard]] std::string_view toString(DeviceState state);

enum class Severity { Info, Warning, Critical, Unknown };

[[nodiscard]] std::string_view toString(Severity severity);

enum class AlarmType { Occlusion, LowPressure, DeviceFailure, Unknown };

[[nodiscard]] std::string_view toString(AlarmType type);

#endif  // STATUS_HPP