#include "message_generator.hpp"

#include <array>
#include <chrono>
#include <iomanip>
#include <nlohmann/json.hpp>

MessageGenerator::MessageGenerator(std::string deviceID)
    : deviceID_(std::move(deviceID)), rng_(std::random_device{}()) {}

std::string MessageGenerator::randomTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&t), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string MessageGenerator::randomStatus() {
    static const std::array<std::string, 5> statuses{"Disconnected", "Connecting", "Connected",
                                                     "Error", "Unknown"};

    std::uniform_int_distribution<size_t> dist(0, statuses.size() - 1);
    return statuses[dist(rng_)];
}

double MessageGenerator::randomFlowRate() {
    std::uniform_real_distribution<double> dist(0.0, 5.0);
    return dist(rng_);
}

double MessageGenerator::randomPressure() {
    std::uniform_real_distribution<double> dist(0.0, 4.0);
    return dist(rng_);
}

std::optional<MessageGenerator::SimulatedAlarm> MessageGenerator::randomAlarm() {
    std::bernoulli_distribution hasAlarm(0.5);
    if (!hasAlarm(rng_)) {
        return std::nullopt;
    }

    static const std::array<std::string, 4> types{"Occlusion", "Low_Pressure", "Device_Failure",
                                                  "Unknown"};
    static const std::array<std::string, 4> severities{"Info", "Warning", "Critical", "Unknown"};

    std::uniform_int_distribution<size_t> typeDist(0, types.size() - 1);
    std::uniform_int_distribution<size_t> sevDist(0, severities.size() - 1);

    return SimulatedAlarm{types[typeDist(rng_)], severities[sevDist(rng_)]};
}

std::string MessageGenerator::generate() {
    nlohmann::json message = {{"device_id", deviceID_},
                              {"timestamp", randomTimestamp()},
                              {"status", randomStatus()},
                              {"flow_rate", randomFlowRate()},
                              {"pressure", randomPressure()}};

    if (const auto alarm = randomAlarm(); alarm.has_value()) {
        message["alarm"] = {{"type", alarm->type}, {"severity", alarm->severity}};
    }

    return message.dump() + '\n';
}