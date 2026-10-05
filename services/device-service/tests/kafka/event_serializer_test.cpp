#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "device/domain/alarm.hpp"
#include "device/domain/device_id.hpp"
#include "device/domain/device_status.hpp"
#include "device/kafka/event_serializer.hpp"

TEST(EventSerializerTest, SerializeMeasurement) {
    Measurement measurement(DeviceID{"pump-b-001"}, 1.5, 2.1,
                            std::chrono::system_clock::time_point(std::chrono::milliseconds(1000)));
    std::string serialized = EventSerializer::serialize(measurement);

    const auto json = nlohmann::json::parse(serialized);

    EXPECT_EQ(json["event_type"], "measurement");
    EXPECT_EQ(json["device_id"], "pump-b-001");
    EXPECT_EQ(json["flow_rate"], 1.5);
    EXPECT_EQ(json["pressure"], 2.1);
}

TEST(EventSerializerTest, SerializeAlarm) {
    Alarm alarm(DeviceID{"pump-b-001"}, Severity::Critical, AlarmType::Occlusion,
                std::chrono::system_clock::time_point(std::chrono::milliseconds(1000)));
    std::string serialized = EventSerializer::serialize(alarm);

    const auto json = nlohmann::json::parse(serialized);

    EXPECT_EQ(json["event_type"], "alarm");
    EXPECT_EQ(json["device_id"], "pump-b-001");
    EXPECT_EQ(json["alarm_type"], "Occlusion");
    EXPECT_EQ(json["severity"], "Critical");
}

TEST(EventSerializerTest, SerializeDeviceStatus) {
    DeviceStatus status(DeviceID{"pump-b-001"}, DeviceState::Connected,
                        std::chrono::system_clock::time_point(std::chrono::milliseconds(1000)));
    std::string serialized = EventSerializer::serialize(status);

    const auto json = nlohmann::json::parse(serialized);

    EXPECT_EQ(json["event_type"], "device_status");
    EXPECT_EQ(json["device_id"], "pump-b-001");
    EXPECT_EQ(json["status"], "Connected");
}