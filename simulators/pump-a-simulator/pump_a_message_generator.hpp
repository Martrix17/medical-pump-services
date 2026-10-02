#ifndef PUMP_A_MESSAGE_GENERATOR_HPP
#define PUMP_A_MESSAGE_GENERATOR_HPP

#include <optional>
#include <random>
#include <string>

#include "simulator/interface_message_generator.hpp"

class PumpAMessageGenerator : public IMessageGenerator {
  public:
    explicit PumpAMessageGenerator(std::string deviceID);

    [[nodiscard]] std::string generate() override;

  private:
    struct SimulatedMeasurement {
        double flowRate;
        double pressure;
    };

    struct SimulatedAlarm {
        std::string type;
        std::string severity;
    };

    std::string deviceID_;
    std::mt19937 rng_;

    [[nodiscard]] static std::string randomTimestamp();
    [[nodiscard]] std::string randomStatus();
    [[nodiscard]] std::optional<SimulatedMeasurement> randomMeasurement();
    [[nodiscard]] std::optional<SimulatedAlarm> randomAlarm();
};

#endif  // PUMP_A_MESSAGE_GENERATOR_HPP