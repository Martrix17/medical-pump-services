#include <cmath>
#include <stdexcept>

#include "device/domain/measurement.hpp"

Measurement::Measurement(
    DeviceID deviceID,
    double flowRate,
    double pressure,
    Timestamp timestamp
) : deviceID_(std::move(deviceID)),
    flowRate_(flowRate), 
    pressure_(pressure),
    timestamp_(timestamp) 
{
    if (!std::isfinite(flowRate_)) {
        throw std::invalid_argument("Flow rate must be finite");
    }
    if (!std::isfinite(pressure_)) {
        throw std::invalid_argument("Pressure must be finite");
    }
};

const DeviceID& Measurement::deviceID() const noexcept
{
    return deviceID_;
}

const double Measurement::flowRate() const noexcept
{
    return flowRate_;
}

const double Measurement::pressure() const noexcept
{
    return pressure_;
}

Timestamp Measurement::timestamp() const noexcept
{
    return timestamp_;
}