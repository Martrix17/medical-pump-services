#ifndef MESSAGE_GENERATOR_HPP
#define MESSAGE_GENERATOR_HPP

#include <optional>
#include <random>
#include <string>

class MessageGenerator {
  public:
    explicit MessageGenerator(std::string deviceID);

    [[nodiscard]] std::string generate();

  private:
    struct SimulatedAlarm {
        std::string type;
        std::string severity;
    };

    std::string deviceID_;
    std::mt19937 rng_;

    [[nodiscard]] static std::string randomTimestamp();
    [[nodiscard]] std::string randomStatus();
    [[nodiscard]] double randomFlowRate();
    [[nodiscard]] double randomPressure();
    [[nodiscard]] std::optional<SimulatedAlarm> randomAlarm();
};

#endif  // MESSAGE_GENERATOR_HPP