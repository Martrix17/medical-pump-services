#include <limits>

#include <gtest/gtest.h>

#include "device/domain/alarm.hpp"

TEST(AlarmConstructorTest, ValidAlarm) {
    DeviceID deviceID{"device-1"};
    Severity severity = Severity::Warning;
    AlarmType type = AlarmType::LowPressure;
    Timestamp timestamp = Timestamp::clock::now();

    Alarm alarm(deviceID, severity, type, timestamp);

    EXPECT_EQ(alarm.deviceID(), deviceID);
    EXPECT_EQ(alarm.severity(), severity);
    EXPECT_EQ(alarm.type(), type);
    EXPECT_EQ(alarm.timestamp(), timestamp);
}
