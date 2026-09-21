#include <limits>

#include <gtest/gtest.h>

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

TEST(MeasurmentQualityTest, InvalidFlowRate) {
    DeviceID deviceID{"device-1"};
    double pressure = 2.0;
    Timestamp timestamp = Timestamp::clock::now();    
    double flowRate = std::numeric_limits<double>::infinity();

    EXPECT_THROW((Measurement{deviceID, flowRate, pressure, timestamp}), std::invalid_argument);

    double flowRate2 = std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW((Measurement{deviceID, flowRate2, pressure, timestamp}), std::invalid_argument);
}

TEST(MeasurmentQualityTest, InvalidPressure) {
    DeviceID deviceID{"device-1"};
    double flowRate = 1.0;
    Timestamp timestamp = Timestamp::clock::now();    
    double pressure = std::numeric_limits<double>::infinity();

    EXPECT_THROW((Measurement{deviceID, flowRate, pressure, timestamp}), std::invalid_argument);

    double pressure2 = std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW((Measurement{deviceID, flowRate, pressure2, timestamp}), std::invalid_argument);
}
