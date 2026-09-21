#include <stdexcept>
#include <algorithm>

#include "device/domain/device_id.hpp"

DeviceID::DeviceID(std::string value) : value_(std::move(value))
{
    if (value_.empty() ||
        std::all_of(value_.begin(), value_.end(),
                    [](unsigned char c) { return std::isspace(c); }))
    {
        throw std::invalid_argument("DeviceID cannot be empty");
    }
}

const std::string& DeviceID::value() const noexcept
{
    return value_;
}
