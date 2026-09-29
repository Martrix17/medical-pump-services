#include <gtest/gtest.h>

#include <limits>

#include "device/domain/device_status.hpp"

TEST(DeviceStatusConstructorTest, ValidDeviceStatus) {
    DeviceID deviceID{"device-1"};
    DeviceState state = DeviceState::Connected;
    Timestamp timestamp = Timestamp::clock::now();

    DeviceStatus deviceStatus(deviceID, state, timestamp);

    EXPECT_EQ(deviceStatus.deviceID(), deviceID);
    EXPECT_EQ(deviceStatus.deviceState(), state);
    EXPECT_EQ(deviceStatus.timestamp(), timestamp);
}
