#ifndef DEVICE_STATUS_PARSER_HPP
#define DEVICE_STATUS_PARSER_HPP

#include <string_view>

#include "device/domain/status.hpp"

namespace device::common {
DeviceStatus parseDeviceStatus(std::string_view status);
}

#endif  // DEVICE_STATUS_PARSER_HPP