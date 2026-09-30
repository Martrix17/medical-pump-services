#ifndef PUMP_B_PROTOCOL_HPP
#define PUMP_B_PROTOCOL_HPP

#include <optional>

#include "device/adapter/pump_b_message.hpp"
#include "device/domain/state_enums.hpp"

namespace device::pumpB {
[[nodiscard]] std::optional<PumpBMessageType> parseMessageType(std::uint8_t value);
[[nodiscard]] std::optional<PumpBStatus> parseStatus(std::uint8_t value);
[[nodiscard]] std::optional<PumpBAlarmType> parseAlarmType(std::uint8_t value);
[[nodiscard]] std::optional<PumpBSeverity> parseSeverity(std::uint8_t value);

[[nodiscard]] DeviceState toDomain(PumpBStatus status);
[[nodiscard]] AlarmType toDomain(PumpBAlarmType type);
[[nodiscard]] Severity toDomain(PumpBSeverity severity);
}  // namespace device::pumpB

#endif  // PUMP_B_PROTOCOL_HPP
