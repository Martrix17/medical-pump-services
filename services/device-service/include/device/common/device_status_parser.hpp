#ifndef DEVICE_STATUS_PARSER_HPP
#define DEVICE_STATUS_PARSER_HPP

#include <string_view>

#include "device/domain/state_enums.hpp"

namespace device::common {
DeviceState parseDeviceState(std::string_view status);
}

#endif  // DEVICE_STATUS_PARSER_HPP