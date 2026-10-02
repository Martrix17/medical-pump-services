#include "device/event/console_event_sink.hpp"

#include <iostream>
#include <utility>
#include <variant>

#include "device/domain/event.hpp"
#include "device/domain/state_enums.hpp"

void ConsoleEventSink::publish(DeviceEvent event) {
    std::visit(
        [](const auto& event) {
            using EventType = std::decay_t<decltype(event)>;

            if constexpr (std::is_same_v<EventType, Measurement>) {
                std::cout << "[Measurement] " << event.deviceID().value() << ": "
                          << "flow rate: " << event.flowRate() << ", pressure: " << event.pressure()
                          << '\n';
            } else if constexpr (std::is_same_v<EventType, Alarm>) {
                std::cout << "[Alarm] " << event.deviceID().value() << ": "
                          << "type: " << toString(event.type())
                          << ", severity: " << toString(event.severity()) << '\n';
            } else if constexpr (std::is_same_v<EventType, DeviceStatus>) {
                std::cout << "[Status] " << event.deviceID().value() << ": "
                          << toString(event.deviceState()) << '\n';
            }
        },
        event);
}