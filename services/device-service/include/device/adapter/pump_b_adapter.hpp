#ifndef PUMP_B_ADAPTER_HPP
#define PUMP_B_ADAPTER_HPP

#include "interface_device_adapter.hpp"
#include "pump_b_message.hpp"

class PumpBAdapter : public IDeviceAdapter {
  public:
    std::vector<DeviceEvent> processMessage(std::string_view rawMessage) override;

    [[nodiscard]] DeviceID getDeviceID() const override;
    [[nodiscard]] std::optional<DeviceStatus> getStatus() const override;
    [[nodiscard]] std::vector<Alarm> getAlarms() const override;
    [[nodiscard]] std::optional<Measurement> getMeasurement() const override;

  private:
    [[nodiscard]] static Timestamp resolveTimestamp(std::uint64_t rawTimestamp);

    std::optional<PumpBMessage> latestMessage_;
};

#endif  // PUMP_B_ADAPTER_HPP
