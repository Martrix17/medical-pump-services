#ifndef PUMP_B_MESSAGE_GENERATOR_HPP
#define PUMP_B_MESSAGE_GENERATOR_HPP

#include <cstdint>
#include <optional>
#include <random>
#include <string>

#include "simulator/interface_message_generator.hpp"

class PumpBMessageGenerator : public IMessageGenerator {
  public:
    explicit PumpBMessageGenerator(std::uint32_t deviceID);

    [[nodiscard]] std::string generate() override;

  private:
    enum class MessageType : std::uint8_t {
        Status = 0x01,
        Measurement = 0x02,
        Alarm = 0x03,
    };

    enum class Status : std::uint8_t {
        Disconnected = 0x00,
        Connecting = 0x01,
        Connected = 0x02,
        Error = 0x03,
        Unknown = 0xFF
    };

    enum class AlarmType : std::uint8_t {
        None = 0x00,
        Occlusion = 0x01,
        LowPressure = 0x02,
        DeviceFailure = 0x03,
        Unknown = 0x04,
    };

    enum class Severity : std::uint8_t {
        None = 0x00,
        Info = 0x01,
        Warning = 0x02,
        Critical = 0x03
    };

    struct SimulatedMeasurement {
        float flowRate;
        float pressure;
    };

    struct SimulatedAlarm {
        AlarmType type;
        Severity severity;
    };

    std::uint32_t deviceID_;
    std::mt19937 rng_;

    [[nodiscard]] static std::uint64_t randomTimestamp();
    [[nodiscard]] Status randomStatus();
    [[nodiscard]] MessageType randomMessageType();
    [[nodiscard]] std::optional<SimulatedMeasurement> randomMeasurement();
    [[nodiscard]] std::optional<SimulatedAlarm> randomAlarm();
};

#endif  // PUMP_B_MESSAGE_GENERATOR_HPP