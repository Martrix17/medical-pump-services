#include <gtest/gtest.h>

#include <string_view>

#include "device/adapter/pump_a_adapter.hpp"
#include "device/domain/alarm.hpp"
#include "device/domain/measurement.hpp"
#include "device/domain/status.hpp"

namespace {

constexpr std::string_view testMessage = R"({
    "device_id": "pump-a-001",
    "timestamp": "2026-09-22T00:00:00Z",
    "status": "Connected",
    "flow_rate": 1.5,
    "pressure": 2.1,
    "alarm": {
        "type": "Low_Pressure",
        "severity": "Warning"
    }
})";

class PumpAAdapterTest : public ::testing::Test {
  protected:
    void SetUp() override {
        adapter.processMessage(testMessage);
    }

    PumpAAdapter adapter;
};

TEST_F(PumpAAdapterTest, GetsDeviceId) {
    const auto deviceID = adapter.getDeviceID();

    ASSERT_TRUE(!deviceID.value().empty());
    EXPECT_EQ(deviceID.value(), "pump-a-001");
}

TEST_F(PumpAAdapterTest, GetsMeasurement) {
    const auto measurement = adapter.getMeasurement();

    ASSERT_TRUE(measurement.has_value());
    EXPECT_DOUBLE_EQ(measurement->flowRate(), 1.5);
    EXPECT_DOUBLE_EQ(measurement->pressure(), 2.1);
}

TEST_F(PumpAAdapterTest, GetsStatus) {
    const auto status = adapter.getStatus();

    EXPECT_EQ(status, DeviceStatus::Connected);
}

TEST_F(PumpAAdapterTest, GetsAlarm) {
    const auto alarms = adapter.getAlarms();

    ASSERT_FALSE(alarms.empty());

    const auto& alarm = alarms.front();

    EXPECT_EQ(alarm.type(), AlarmType::LowPressure);
    EXPECT_EQ(alarm.severity(), Severity::Warning);
}

TEST_F(PumpAAdapterTest, RejectsEmptyMessage) {
    PumpAAdapter emptyAdapter;

    EXPECT_FALSE(emptyAdapter.processMessage(""));
}

}  // namespace