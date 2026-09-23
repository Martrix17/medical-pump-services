#ifndef INTERFACE_DEVICE_ADAPTER_HPP
#define INTERFACE_DEVICE_ADAPTER_HPP

#include <optional>
#include <vector>

#include "device/domain/alarm.hpp"
#include "device/domain/device_id.hpp"
#include "device/domain/measurement.hpp"
#include "device/domain/status.hpp"

class InterfaceDeviceAdapter {
  public:
    virtual ~InterfaceDeviceAdapter() = default;

    virtual DeviceID getDeviceID() = 0;
    virtual DeviceStatus getStatus() = 0;
    virtual std::vector<Alarm> getAlarms() = 0;
    virtual std::optional<Measurement> getMeasurement() = 0;
};

#endif  // INTERFACE_DEVICE_ADAPTER_HPP
