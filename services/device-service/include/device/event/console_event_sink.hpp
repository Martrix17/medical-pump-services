#ifndef CONSOLE_EVENT_SINK_HPP
#define CONSOLE_EVENT_SINK_HPP

#include <vector>

#include "device/domain/event.hpp"
#include "interface_event_sink.hpp"

class ConsoleEventSink : public IEventSink {
  public:
    void publish(DeviceEvent event) override;
};

#endif  // CONSOLE_EVENT_SINK_HPP