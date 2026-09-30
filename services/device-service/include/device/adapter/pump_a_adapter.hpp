#ifndef PUMP_A_ADAPTER_HPP
#define PUMP_A_ADAPTER_HPP

#include "interface_device_adapter.hpp"
#include "pump_a_message.hpp"

class PumpAAdapter : public IDeviceAdapter {
  public:
    std::vector<DeviceEvent> processMessage(std::string_view rawMessage) override;

    [[nodiscard]] DeviceID getDeviceID() const override;
    [[nodiscard]] std::optional<DeviceStatus> getStatus() const override;
    [[nodiscard]] std::vector<Alarm> getAlarms() const override;
    [[nodiscard]] std::optional<Measurement> getMeasurement() const override;

  private:
    [[nodiscard]] static Timestamp resolveTimestamp(const std::string& rawTimestamp);

    std::optional<PumpAMessage> latestMessage_;
};

#endif  // PUMP_A_ADAPTER_HPP
