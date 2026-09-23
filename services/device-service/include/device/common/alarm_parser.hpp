#ifndef ALARM_PARSER_HPP
#define ALARM_PARSER_HPP

#include <string_view>

#include "device/domain/status.hpp"

namespace device::common {
Severity parseSeverity(std::string_view severity);
AlarmType parseAlarmType(std::string_view type);
}  // namespace device::common

#endif  // ALARM_PARSER_HPP