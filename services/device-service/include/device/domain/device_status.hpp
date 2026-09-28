#ifndef DEVICE_STATUS_HPP
#define DEVICE_STATUS_HPP

#include <chrono>

#include "device_id.hpp"
#include "state_enums.hpp"

using Timestamp = std::chrono::system_clock::time_point;

class DeviceStatus {
  public:
    explicit DeviceStatus(DeviceID deviceID, State state, Timestamp timestamp);

    [[nodiscard]] const DeviceID& deviceID() const noexcept;
    [[nodiscard]] State state() const noexcept;
    [[nodiscard]] Timestamp timestamp() const noexcept;

  private:
    DeviceID deviceID_;
    State state_;
    Timestamp timestamp_;
};

#endif  // DEVICE_STATUS_HPP