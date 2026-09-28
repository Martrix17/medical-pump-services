#ifndef PUMP_A_MESSAGE_HPP
#define PUMP_A_MESSAGE_HPP

#include <optional>
#include <string>

struct PumpAAlarm {
    std::string type;
    std::string severity;
};

struct PumpAMessage {
    std::string deviceID;
    std::string timestamp;
    std::string status;
    double flowRate;
    double pressure;
    std::optional<PumpAAlarm> alarm;
};

#endif  // PUMP_A_MESSAGE_HPP