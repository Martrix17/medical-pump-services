#include "device/kafka/event_serializer.hpp"

#include <nlohmann/json.hpp>
#include <stdexcept>
#include <variant>

#include "device/domain/alarm.hpp"
#include "device/domain/device_status.hpp"
#include "device/domain/measurement.hpp"

namespace {

using json = nlohmann::json;

json serializeMeasurement(const Measurement& measurement) {
    return {{"event_type", "measurement"},
            {"device_id", measurement.deviceID().value()},
            {"timestamp", measurement.timestamp().time_since_epoch().count()},
            {"flow_rate", measurement.flowRate()},
            {"pressure", measurement.pressure()}};
}

json serializeAlarm(const Alarm& alarm) {
    return {{"event_type", "alarm"},
            {"device_id", alarm.deviceID().value()},
            {"timestamp", alarm.timestamp().time_since_epoch().count()},
            {"alarm_type", toString(alarm.type())},
            {"severity", toString(alarm.severity())}};
}

json serializeDeviceStatus(const DeviceStatus& status) {
    return {{"event_type", "device_status"},
            {"device_id", status.deviceID().value()},
            {"timestamp", status.timestamp().time_since_epoch().count()},
            {"status", toString(status.deviceState())}};
}

}  // namespace

std::string EventSerializer::serialize(const DeviceEvent& event) {
    const auto jsonEvent = std::visit(
        [](const auto& concreteEvent) -> json {
            using T = std::decay_t<decltype(concreteEvent)>;

            if constexpr (std::is_same_v<T, Measurement>) {
                return serializeMeasurement(concreteEvent);
            } else if constexpr (std::is_same_v<T, Alarm>) {
                return serializeAlarm(concreteEvent);
            } else if constexpr (std::is_same_v<T, DeviceStatus>) {
                return serializeDeviceStatus(concreteEvent);
            }
        },
        event);

    return jsonEvent.dump();
}