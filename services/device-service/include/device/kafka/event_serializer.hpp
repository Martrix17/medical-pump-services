#ifndef EVENT_SEIALIZER_HPP
#define EVENT_SEIALIZER_HPP

#include <string>

#include "device/domain/event.hpp"

class EventSerializer {
  public:
    [[nodiscard]] static std::string serialize(const DeviceEvent& event);
};

#endif  // EVENT_SEIALIZER_HPP