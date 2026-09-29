#ifndef DEVICE_STATUS_HPP
#define DEVICE_STATUS_HPP

#include <chrono>

#include "device_id.hpp"
#include "state_enums.hpp"

using Timestamp = std::chrono::system_clock::time_point;

class DeviceStatus {
  public:
    explicit DeviceStatus(DeviceID deviceID, DeviceState deviceState, Timestamp timestamp);

    [[nodiscard]] const DeviceID& deviceID() const noexcept;
    [[nodiscard]] DeviceState deviceState() const noexcept;
    [[nodiscard]] Timestamp timestamp() const noexcept;

  private:
    DeviceID deviceID_;
    DeviceState deviceState_;
    Timestamp timestamp_;
};

#endif  // DEVICE_STATUS_HPP