#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "device/adapter/pump_a_adapter.hpp"
#include "device/common/device_status_parser.hpp"
#include "device/domain/alarm.hpp"
#include "device/domain/device_status.hpp"
#include "device/domain/measurement.hpp"

namespace {

using json = nlohmann::json;

constexpr std::string_view kMessageWithAlarm = R"({
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

json alarmMessage() {
    return json::parse(kMessageWithAlarm);
}

json baseMessage() {
    auto message = alarmMessage();
    message.erase("alarm");
    return message;
}

template <typename T> std::size_t countOf(const std::vector<DeviceEvent>& events) {
    return static_cast<std::size_t>(
        std::count_if(events.begin(), events.end(),
                      [](const DeviceEvent& e) { return std::holds_alternative<T>(e); }));
}

std::optional<Measurement> firstMeasurement(const std::vector<DeviceEvent>& events) {
    for (const auto& e : events) {
        if (const auto* m = std::get_if<Measurement>(&e)) {
            return *m;
        }
    }
    return std::nullopt;
}

}  // namespace

TEST(PumpAAdapterTest, FullDeviceMessageYieldsStatusMeasurementAndAlarm) {
    PumpAAdapter adapter;
    const auto events = adapter.processMessage(kMessageWithAlarm);

    EXPECT_EQ(events.size(), 3U);
    EXPECT_EQ(countOf<DeviceStatus>(events), 1U);
    EXPECT_EQ(countOf<Measurement>(events), 1U);
    EXPECT_EQ(countOf<Alarm>(events), 1U);
}

TEST(PumpAAdapterTest, MessageWithoutAlarmYieldsStatusAndMeasurementOnly) {
    PumpAAdapter adapter;
    const auto events = adapter.processMessage(baseMessage().dump());

    EXPECT_EQ(events.size(), 2U);
    EXPECT_EQ(countOf<Alarm>(events), 0U);
}

TEST(PumpAAdapterTest, NullAlarmYieldsNoAlarmEvent) {
    auto message = baseMessage();
    message["alarm"] = nullptr;

    PumpAAdapter adapter;
    EXPECT_EQ(countOf<Alarm>(adapter.processMessage(message.dump())), 0U);
}

TEST(PumpAAdapterTest, MeasurementCarriesFlowRateAndPressure) {
    PumpAAdapter adapter;
    const auto measurement = firstMeasurement(adapter.processMessage(kMessageWithAlarm));

    ASSERT_TRUE(measurement.has_value());
    EXPECT_DOUBLE_EQ(measurement->flowRate(), 1.5);
    EXPECT_DOUBLE_EQ(measurement->pressure(), 2.1);
}

TEST(PumpAAdapterTest, TimestampIsParsedExactly) {
    PumpAAdapter adapter;
    const auto measurement = firstMeasurement(adapter.processMessage(kMessageWithAlarm));

    ASSERT_TRUE(measurement.has_value());
    const Timestamp expected{
        std::chrono::sys_days{std::chrono::year{2026} / std::chrono::September / 22}};
    EXPECT_EQ(measurement->timestamp(), expected);
}

TEST(PumpAAdapterTest, UnparsableTimestampFallsBackToProcessingTime) {
    auto message = baseMessage();
    message["timestamp"] = "not-a-timestamp";

    PumpAAdapter adapter;
    const auto before = Timestamp::clock::now();
    const auto measurement = firstMeasurement(adapter.processMessage(message.dump()));
    const auto after = Timestamp::clock::now();

    ASSERT_TRUE(measurement.has_value());
    EXPECT_GE(measurement->timestamp(), before);
    EXPECT_LE(measurement->timestamp(), after);
}

TEST(PumpAAdapterTest, StatusMatchesStatusParser) {
    PumpAAdapter adapter;
    const auto events = adapter.processMessage(kMessageWithAlarm);

    ASSERT_EQ(countOf<DeviceStatus>(events), 1U);
    const auto status = std::get<DeviceStatus>(
        *std::find_if(events.begin(), events.end(), [](const DeviceEvent& e) {
            return std::holds_alternative<DeviceStatus>(e);
        }));
    EXPECT_EQ(status.state(), device::common::parseState("Connected"));
    EXPECT_NE(status.state(), State::Unknown);
}

TEST(PumpAAdapterTest, UnknownStatusStringYieldsUnknownStatusAndKeepsMeasurement) {
    auto message = baseMessage();
    message["status"] = "definitely-not-a-status";

    PumpAAdapter adapter;
    const auto events = adapter.processMessage(message.dump());

    EXPECT_EQ(countOf<Measurement>(events), 1U);
    ASSERT_EQ(countOf<DeviceStatus>(events), 1U);
    EXPECT_EQ(std::get<DeviceStatus>(events.front()).state(), State::Unknown);
}

class PumpAAdapterMalformedTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PumpAAdapterMalformedTest, YieldsNoEvents) {
    PumpAAdapter adapter;
    EXPECT_TRUE(adapter.processMessage(GetParam()).empty());
}

INSTANTIATE_TEST_SUITE_P(BadInput, PumpAAdapterMalformedTest,
                         ::testing::Values(
                             std::string{""}, std::string{"not json"}, std::string{"{}"},
                             std::string{"[]"},
                             [] {
                                 auto m = baseMessage();
                                 m.erase("flow_rate");
                                 return m.dump();
                             }(),
                             [] {
                                 auto m = baseMessage();
                                 m["flow_rate"] = "fast";
                                 return m.dump();
                             }(),
                             [] {
                                 auto m = baseMessage();
                                 m.erase("device_id");
                                 return m.dump();
                             }(),
                             [] {
                                 auto m = alarmMessage();
                                 m["alarm"].erase("severity");
                                 return m.dump();
                             }()));

TEST(PumpAAdapterTest, FailedParseDoesNotReuseStateFromPreviousMessage) {
    PumpAAdapter adapter;
    ASSERT_FALSE(adapter.processMessage(kMessageWithAlarm).empty());

    EXPECT_TRUE(adapter.processMessage("garbage").empty());
}

TEST(PumpAAdapterTest, AdapterIsReusableAcrossMessages) {
    auto second = baseMessage();
    second["flow_rate"] = 99.0;

    PumpAAdapter adapter;
    adapter.processMessage(kMessageWithAlarm);
    const auto measurement = firstMeasurement(adapter.processMessage(second.dump()));

    ASSERT_TRUE(measurement.has_value());
    EXPECT_DOUBLE_EQ(measurement->flowRate(), 99.0);
}

TEST(PumpAAdapterTest, AlarmDoesNotLeakIntoFollowingAlarmFreeMessage) {
    PumpAAdapter adapter;
    ASSERT_EQ(countOf<Alarm>(adapter.processMessage(kMessageWithAlarm)), 1U);

    EXPECT_EQ(countOf<Alarm>(adapter.processMessage(baseMessage().dump())), 0U);
}
