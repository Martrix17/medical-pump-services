#include <gtest/gtest.h>

#include <limits>

#include "device/domain/measurement.hpp"

TEST(MeasurementConstructorTest, ValidMeasurement) {
    DeviceID deviceID{"device-1"};
    double flowRate = 1.0;
    double pressure = 2.0;
    Timestamp timestamp = Timestamp::clock::now();

    Measurement measurement(deviceID, flowRate, pressure, timestamp);

    EXPECT_EQ(measurement.deviceID(), deviceID);
    EXPECT_DOUBLE_EQ(measurement.flowRate(), flowRate);
    EXPECT_DOUBLE_EQ(measurement.pressure(), pressure);
    EXPECT_EQ(measurement.timestamp(), timestamp);
}

TEST(MeasurmentQualityTest, InfiniteFlowRate) {
    DeviceID deviceID{"device-1"};
    double pressure = 2.0;
    Timestamp timestamp = Timestamp::clock::now();
    double flowRate = std::numeric_limits<double>::infinity();

    EXPECT_THROW((Measurement{deviceID, flowRate, pressure, timestamp}), std::invalid_argument);
}

TEST(MeasurmentQualityTest, NaNFlowRate) {
    DeviceID deviceID{"device-1"};
    double pressure = 2.0;
    Timestamp timestamp = Timestamp::clock::now();
    double flowRate = std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW((Measurement{deviceID, flowRate, pressure, timestamp}), std::invalid_argument);
}

TEST(MeasurmentQualityTest, InfinitePressure) {
    DeviceID deviceID{"device-1"};
    double flowRate = 1.0;
    Timestamp timestamp = Timestamp::clock::now();
    double pressure = std::numeric_limits<double>::infinity();

    EXPECT_THROW((Measurement{deviceID, flowRate, pressure, timestamp}), std::invalid_argument);
}

TEST(MeasurmentQualityTest, NaNPressure) {
    DeviceID deviceID{"device-1"};
    double flowRate = 1.0;
    Timestamp timestamp = Timestamp::clock::now();
    double pressure = std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW((Measurement{deviceID, flowRate, pressure, timestamp}), std::invalid_argument);
}
