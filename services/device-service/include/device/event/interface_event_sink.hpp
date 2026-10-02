#ifndef INTERFACE_EVENT_SINK_HPP
#define INTERFACE_EVENT_SINK_HPP

#include "device/domain/event.hpp"

class IEventSink {
  public:
    virtual ~IEventSink() = default;

    virtual void publish(DeviceEvent event) = 0;
};

#endif  // INTERFACE_EVENT_SINK_HPP