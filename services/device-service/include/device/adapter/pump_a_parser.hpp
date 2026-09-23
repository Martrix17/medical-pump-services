#ifndef PUMP_A_PARSER_HPP
#define PUMP_A_PARSER_HPP

#include <optional>
#include <string_view>

#include "device/adapter/pump_a_message.hpp"

class PumpAParser {
  public:
    [[nodiscard]] static std::optional<PumpAMessage> parse(std::string_view rawMessage);
};

#endif  //  PUMP_A_PARSER_HPP