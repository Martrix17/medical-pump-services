#ifndef PUMP_A_ADAPTER_HPP
#define PUMP_A_ADAPTER_HPP

#include "interface_device_adapter.hpp"
#include "pump_a_message.hpp"

class PumpAAdapter : public InterfaceDeviceAdapter {
  public:
    bool processMessage(std::string_view rawMessage);

    DeviceID getDeviceID() override;

    DeviceStatus getStatus() override;

    std::vector<Alarm> getAlarms() override;

    std::optional<Measurement> getMeasurement() override;

  private:
    std::optional<PumpAMessage> latestMessage_;
};

#endif  // PUMP_A_ADAPTER_HPP
