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

TEST_F(PumpAAdapterTest, RejectsMalformedJson) {
    PumpAAdapter malformedAdapter;

    EXPECT_FALSE(malformedAdapter.processMessage("{not valid json"));
}

TEST_F(PumpAAdapterTest, RejectsMessageMissingRequiredField) {
    constexpr std::string_view missingFlowRate = R"({
        "device_id": "pump-a-002",
        "timestamp": "2026-09-22T00:00:00Z",
        "status": "Connected",
        "pressure": 2.1
    })";

    PumpAAdapter incompleteAdapter;

    EXPECT_FALSE(incompleteAdapter.processMessage(missingFlowRate));
}

TEST_F(PumpAAdapterTest, NoAlarmWhenFieldAbsent) {
    constexpr std::string_view noAlarmMessage = R"({
        "device_id": "pump-a-003",
        "timestamp": "2026-09-22T00:00:00Z",
        "status": "Connected",
        "flow_rate": 1.0,
        "pressure": 1.0
    })";

    PumpAAdapter noAlarmAdapter;
    ASSERT_TRUE(noAlarmAdapter.processMessage(noAlarmMessage));

    EXPECT_TRUE(noAlarmAdapter.getAlarms().empty());
    ASSERT_TRUE(noAlarmAdapter.getMeasurement().has_value());
    EXPECT_EQ(noAlarmAdapter.getStatus(), DeviceStatus::Connected);
}

TEST_F(PumpAAdapterTest, NoAlarmWhenFieldIsNull) {
    constexpr std::string_view nullAlarmMessage = R"({
        "device_id": "pump-a-004",
        "timestamp": "2026-09-22T00:00:00Z",
        "status": "Connected",
        "flow_rate": 1.0,
        "pressure": 1.0,
        "alarm": null
    })";

    PumpAAdapter nullAlarmAdapter;
    ASSERT_TRUE(nullAlarmAdapter.processMessage(nullAlarmMessage));

    EXPECT_TRUE(nullAlarmAdapter.getAlarms().empty());
}

TEST_F(PumpAAdapterTest, UnknownStatusMapsToUnknownEnum) {
    constexpr std::string_view unknownStatusMessage = R"({
        "device_id": "pump-a-005",
        "timestamp": "2026-09-22T00:00:00Z",
        "status": "UnrecognizedStatus",
        "flow_rate": 1.0,
        "pressure": 1.0
    })";

    PumpAAdapter unknownStatusAdapter;
    ASSERT_TRUE(unknownStatusAdapter.processMessage(unknownStatusMessage));

    EXPECT_EQ(unknownStatusAdapter.getStatus(), DeviceStatus::Unknown);
}

TEST_F(PumpAAdapterTest, InvalidTimestampFallsBackToNow) {
    constexpr std::string_view badTimestampMessage = R"({
        "device_id": "pump-a-006",
        "timestamp": "not-a-real-timestamp",
        "status": "Connected",
        "flow_rate": 1.0,
        "pressure": 1.0,
        "alarm": {
            "type": "Low_Pressure",
            "severity": "Warning"
        }
    })";

    PumpAAdapter badTimestampAdapter;
    ASSERT_TRUE(badTimestampAdapter.processMessage(badTimestampMessage));

    const auto alarms = badTimestampAdapter.getAlarms();
    ASSERT_FALSE(alarms.empty());
    EXPECT_LE(alarms.front().timestamp(), Timestamp::clock::now());
}

TEST_F(PumpAAdapterTest, ReprocessingOverwritesPreviousState) {
    constexpr std::string_view secondMessage = R"({
        "device_id": "pump-a-999",
        "timestamp": "2026-09-22T01:00:00Z",
        "status": "Disconnected",
        "flow_rate": 9.9,
        "pressure": 9.9
    })";

    ASSERT_TRUE(adapter.processMessage(secondMessage));

    EXPECT_EQ(adapter.getDeviceID().value(), "pump-a-999");
    EXPECT_EQ(adapter.getStatus(), DeviceStatus::Disconnected);
    EXPECT_TRUE(adapter.getAlarms().empty());
}

}  // namespace