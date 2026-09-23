#ifndef TIMESTAMP_PARSER_HPP
#define TIMESTAMP_PARSER_HPP

#include <chrono>
#include <optional>
#include <string_view>


using Timestamp = std::chrono::system_clock::time_point;

namespace device::common {
std::optional<Timestamp> parseTimestamp(std::string_view value);
}

#endif  // TIMESTAMP_PARSER_HPP