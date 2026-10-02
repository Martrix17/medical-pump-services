#include "device/adapter/pump_a_parser.hpp"

#include <nlohmann/json.hpp>

std::optional<PumpAMessage> PumpAParser::parse(std::string_view rawMessage) {
    if (rawMessage.empty()) {
        return std::nullopt;
    }

    try {
        nlohmann::json jsonMessage = nlohmann::json::parse(rawMessage);

        PumpAMessage parsedMessage{.deviceID = jsonMessage.at("device_id").get<std::string>(),
                                   .timestamp = jsonMessage.at("timestamp").get<std::string>(),
                                   .status = jsonMessage.at("status").get<std::string>()};

        if (jsonMessage.contains("measurement") && !jsonMessage.at("measurement").is_null()) {
            parsedMessage.measurement = PumpAMeasurement{
                .flowRate = jsonMessage.at("measurement").at("flow_rate").get<double>(),
                .pressure = jsonMessage.at("measurement").at("pressure").get<double>()};
        }

        if (jsonMessage.contains("alarm") && !jsonMessage.at("alarm").is_null()) {
            parsedMessage.alarm =
                PumpAAlarm{.type = jsonMessage.at("alarm").at("type").get<std::string>(),
                           .severity = jsonMessage.at("alarm").at("severity").get<std::string>()};
        }

        return parsedMessage;
    } catch (const nlohmann::json::exception&) {
        return std::nullopt;
    }
}
