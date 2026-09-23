#ifndef MEASUREMENT_HPP
#define MEASUREMENT_HPP

#include <chrono>

#include "device_id.hpp"

using Timestamp = std::chrono::system_clock::time_point;

class Measurement {
  public:
    explicit Measurement(DeviceID deviceID, double flowRate, double pressure, Timestamp timestamp);

    [[nodiscard]] const DeviceID& deviceID() const noexcept;
    [[nodiscard]] double flowRate() const noexcept;
    [[nodiscard]] double pressure() const noexcept;
    [[nodiscard]] Timestamp timestamp() const noexcept;

  private:
    DeviceID deviceID_;
    double flowRate_;
    double pressure_;
    Timestamp timestamp_;
};

#endif  // MEASUREMENT_HPP