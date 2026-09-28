#ifndef EVENT_HPP
#define EVENT_HPP

#include <variant>

#include "alarm.hpp"
#include "device_status.hpp"
#include "measurement.hpp"

using DeviceEvent = std::variant<Measurement, Alarm, DeviceStatus>;

#endif  // EVENT_HPP