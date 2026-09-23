#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <string>

class DeviceID {
  public:
    explicit DeviceID(std::string value);

    [[nodiscard]] const std::string& value() const noexcept;

    friend bool operator==(const DeviceID&, const DeviceID&) = default;

  private:
    std::string value_;
};

#endif  // DEVICE_HPP