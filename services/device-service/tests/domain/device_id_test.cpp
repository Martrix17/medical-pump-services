#include <gtest/gtest.h>

#include "device/domain/device_id.hpp"

TEST(DeviceIDConstructorTest, ValidDeviceID) {
    DeviceID deviceID{"device-1"};
    EXPECT_EQ(deviceID.value(), "device-1");
}

TEST(DeviceIDConstructorTest, EmptyDeviceID) {
    EXPECT_THROW(DeviceID{""}, std::invalid_argument);
}

TEST(DeviceIDConstructorTest, WhitespaceOnlyDeviceID) {
    EXPECT_THROW(DeviceID{"   "}, std::invalid_argument);
    EXPECT_THROW(DeviceID{"\t\n "}, std::invalid_argument);
}

TEST(DeviceIDEqualityTest, EqualDeviceIDs) {
    DeviceID deviceID1{"device-1"};
    DeviceID deviceID2{"device-1"};
    EXPECT_TRUE(deviceID1 == deviceID2);
}

TEST(DeviceIDEqualityTest, NotEqualDeviceIDs) {
    DeviceID deviceID1{"device-1"};
    DeviceID deviceID2{"device-2"};
    EXPECT_FALSE(deviceID1 == deviceID2);
}
