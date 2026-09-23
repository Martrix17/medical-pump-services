#ifndef EVENT_HPP
#define EVENT_HPP

#include <variant>

#include "alarm.hpp"
#include "measurement.hpp"
#include "status.hpp"

using DeviceEvent = std::variant<Measurement, Alarm, DeviceStatus>;

#endif  // EVENT_HPP