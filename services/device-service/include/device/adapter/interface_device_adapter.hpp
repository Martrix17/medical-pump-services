#ifndef INTERFACE_DEVICE_ADAPTER_HPP
#define INTERFACE_DEVICE_ADAPTER_HPP

#include <optional>
#include <vector>

#include "device/domain/alarm.hpp"
#include "device/domain/device_id.hpp"
#include "device/domain/device_status.hpp"
#include "device/domain/event.hpp"
#include "device/domain/measurement.hpp"

class IDeviceAdapter {
  public:
    virtual ~IDeviceAdapter() = default;

    virtual std::vector<DeviceEvent> processMessage(std::string_view rawMessage) = 0;

    [[nodiscard]] virtual DeviceID getDeviceID() const = 0;
    [[nodiscard]] virtual std::optional<DeviceStatus> getStatus() const = 0;
    [[nodiscard]] virtual std::vector<Alarm> getAlarms() const = 0;
    [[nodiscard]] virtual std::optional<Measurement> getMeasurement() const = 0;
};

#endif  // INTERFACE_DEVICE_ADAPTER_HPP
