#ifndef PUMP_A_MESSAGE_HPP
#define PUMP_A_MESSAGE_HPP

#include <optional>
#include <string>

struct PumpAMeasurement {
    double flowRate;
    double pressure;
};

struct PumpAAlarm {
    std::string type;
    std::string severity;
};

struct PumpAMessage {
    std::string deviceID;
    std::string timestamp;
    std::string status;
    std::optional<PumpAMeasurement> measurement;
    std::optional<PumpAAlarm> alarm;
};

#endif  // PUMP_A_MESSAGE_HPP