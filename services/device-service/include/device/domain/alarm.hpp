#ifndef ALARM_HPP
#define ALARM_HPP

#include <chrono>

#include "device_id.hpp"
#include "status.hpp"

using Timestamp = std::chrono::system_clock::time_point;

class Alarm
{
public:
    explicit Alarm(
        DeviceID deviceID, 
        Severity severity, 
        AlarmType type,
        Timestamp timestamp
    );

    [[nodiscard]] const DeviceID& deviceID() const noexcept;
    [[nodiscard]] const Severity severity() const noexcept;
    [[nodiscard]] const AlarmType type() const noexcept;
    [[nodiscard]] Timestamp timestamp() const noexcept;

private:
    DeviceID deviceID_;
    Severity severity_;
    AlarmType type_;
    Timestamp timestamp_;
};

#endif // ALARM_HPP