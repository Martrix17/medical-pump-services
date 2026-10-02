#include "pump_a_message_generator.hpp"

#include <array>
#include <chrono>
#include <iomanip>
#include <nlohmann/json.hpp>

PumpAMessageGenerator::PumpAMessageGenerator(std::string deviceID)
    : deviceID_(std::move(deviceID)), rng_(std::random_device{}()) {}

std::string PumpAMessageGenerator::randomTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&t), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string PumpAMessageGenerator::randomStatus() {
    static const std::array<std::string, 5> statuses{"Disconnected", "Connecting", "Connected",
                                                     "Error", "Unknown"};

    std::uniform_int_distribution<size_t> dist(0, statuses.size() - 1);
    return statuses[dist(rng_)];
}

std::optional<PumpAMessageGenerator::SimulatedMeasurement>
PumpAMessageGenerator::randomMeasurement() {
    std::bernoulli_distribution hasMeasurement(0.75);
    if (!hasMeasurement(rng_)) {
        return std::nullopt;
    }

    std::uniform_real_distribution<double> flow_rates(0.0, 5.0);
    std::uniform_real_distribution<double> pressures(0.0, 4.0);

    return SimulatedMeasurement{flow_rates(rng_), pressures(rng_)};
}

std::optional<PumpAMessageGenerator::SimulatedAlarm> PumpAMessageGenerator::randomAlarm() {
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

std::string PumpAMessageGenerator::generate() {
    nlohmann::json message = {
        {"device_id", deviceID_}, {"timestamp", randomTimestamp()}, {"status", randomStatus()}};

    if (const auto measurement = randomMeasurement(); measurement.has_value()) {
        message["measurement"] = {{"flow_rate", measurement->flowRate},
                                  {"pressure", measurement->pressure}};
    }

    if (const auto alarm = randomAlarm(); alarm.has_value()) {
        message["alarm"] = {{"type", alarm->type}, {"severity", alarm->severity}};
    }

    return message.dump() + '\n';
}