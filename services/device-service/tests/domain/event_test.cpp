#include <gtest/gtest.h>

#include "device/domain/device_status.hpp"
#include "device/domain/event.hpp"
#include "device/domain/state_enums.hpp"

TEST(DeviceEventTest, MeasurementToDeviceEvent) {
    DeviceID deviceID{"device-1"};
    double flowRate = 1.0;
    double pressure = 2.0;
    Timestamp timestamp = Timestamp::clock::now();

    Measurement measurement(deviceID, flowRate, pressure, timestamp);

    DeviceEvent event = measurement;

    EXPECT_TRUE(std::holds_alternative<Measurement>(event));
    EXPECT_DOUBLE_EQ(std::get<Measurement>(event).flowRate(), flowRate);
}

TEST(DeviceEventTest, AlarmToDeviceEvent) {
    DeviceID deviceID{"device-1"};
    Severity severity = Severity::Warning;
    AlarmType type = AlarmType::LowPressure;
    Timestamp timestamp = Timestamp::clock::now();

    Alarm alarm(deviceID, severity, type, timestamp);

    DeviceEvent event = alarm;

    EXPECT_TRUE(std::holds_alternative<Alarm>(event));
    EXPECT_EQ(std::get<Alarm>(event).severity(), severity);
}

TEST(DeviceEventTest, DeviceStatusToDeviceEvent) {
    DeviceID deviceID{"device-1"};
    DeviceState state = DeviceState::Connected;
    Timestamp timestamp = Timestamp::clock::now();

    DeviceStatus status{deviceID, state, timestamp};

    DeviceEvent event = status;

    EXPECT_TRUE(std::holds_alternative<DeviceStatus>(event));
}
