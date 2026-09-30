#ifndef PUMP_B_PARSER_HPP
#define PUMP_B_PARSER_HPP

#include <optional>
#include <string_view>

#include "device/adapter/pump_b_message.hpp"

class PumpBParser {
  public:
    [[nodiscard]] static std::optional<PumpBMessage> parse(std::string_view rawMessage);
};

#endif  //  PUMP_B_PARSER_HPP