#ifndef PUMP_HELPER_HPP
#define PUMP_HELPER_HPP

#include <cstddef>
#include <string>

namespace helper::pumpA {

constexpr std::string_view validPumpAFrame = R"({
    "device_id": "1234",
    "timestamp": "2026-09-22T00:00:00Z",
    "status": "Connected",
    "measurement": {   
        "flow_rate": 1.5,
        "pressure": 2.1
    }
})";

constexpr std::string_view invalidPumpAFrame = R"({
    "device_id": "1234",
    "timestamp": "2026-09-22T00:00:00Z",
    "status": "Connected",
    "measurement": {   
        "flow_rate": "not-a-number",
        "pressure": 2.1
    }
})";

std::string makePumpAFrame(std::string_view rawMessage);

}  // namespace helper::pumpA

namespace helper::pumpB {

constexpr std::size_t PumpBFrameSize = 28;

std::string makeValidPumpBFrame();

}  // namespace helper::pumpB

#endif  // PUMP_HELPER_HPP