#include "device/common/timestamp_parser.hpp"

#include <iomanip>
#include <sstream>
#include <string>

std::optional<Timestamp> device::common::parseTimestamp(std::string_view value) {
    std::tm tm{};
    std::istringstream stream{std::string{value}};

    stream >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");

    if (stream.fail()) {
        return std::nullopt;
    }

    std::time_t time = timegm(&tm);
    if (time == static_cast<std::time_t>(-1)) {
        return std::nullopt;
    }

    return std::chrono::system_clock::from_time_t(time);
}