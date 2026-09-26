#include <gtest/gtest.h>

#include <array>
#include <nlohmann/json.hpp>
#include <ranges>

#include "message_generator.hpp"

namespace {

TEST(MessageGeneratorTest, GeneratesValidJson) {
    MessageGenerator generator{"pump-a-001"};

    const auto message = generator.generate();

    ASSERT_FALSE(message.empty());
    EXPECT_EQ(message.back(), '\n');

    EXPECT_NO_THROW({ nlohmann::json::parse(message); });
}

TEST(MessageGeneratorTest, ContainsRequiredFields) {
    MessageGenerator generator{"pump-a-001"};

    const auto message = generator.generate();
    const auto json = nlohmann::json::parse(message);

    EXPECT_TRUE(json.contains("device_id"));
    EXPECT_TRUE(json.contains("timestamp"));
    EXPECT_TRUE(json.contains("status"));
    EXPECT_TRUE(json.contains("flow_rate"));
    EXPECT_TRUE(json.contains("pressure"));
}

TEST(MessageGeneratorTest, UsesConfiguredDeviceId) {
    MessageGenerator generator{"pump-a-007"};

    const auto message = generator.generate();
    const auto json = nlohmann::json::parse(message);

    ASSERT_TRUE(json.contains("device_id"));
    EXPECT_EQ(json.at("device_id"), "pump-a-007");
}

TEST(MessageGeneratorTest, GeneratesValidMeasurementValues) {
    MessageGenerator generator{"pump-a-001"};

    for (int i = 0; i < 100; ++i) {
        const auto message = generator.generate();
        const auto json = nlohmann::json::parse(message);

        ASSERT_TRUE(json.at("flow_rate").is_number());
        ASSERT_TRUE(json.at("pressure").is_number());

        EXPECT_GE(json.at("flow_rate").get<double>(), 0.0);
        EXPECT_LE(json.at("flow_rate").get<double>(), 5.0);

        EXPECT_GE(json.at("pressure").get<double>(), 0.0);
        EXPECT_LE(json.at("pressure").get<double>(), 4.0);
    }
}

TEST(MessageGeneratorTest, GeneratesValidStatus) {
    constexpr std::array validStatuses{"Disconnected", "Connecting", "Connected", "Error",
                                       "Unknown"};

    MessageGenerator generator{"pump-a-001"};

    for (int i = 0; i < 100; ++i) {
        const auto message = generator.generate();
        const auto json = nlohmann::json::parse(message);

        const auto status = json.at("status").get<std::string>();

        EXPECT_TRUE(std::ranges::find(validStatuses, status) != validStatuses.end());
    }
}

TEST(MessageGeneratorTest, AlarmIsOptional) {
    MessageGenerator generator{"pump-a-001"};

    bool sawAlarm = false;
    bool sawNoAlarm = false;

    for (int i = 0; i < 100; ++i) {
        const auto message = generator.generate();
        const auto json = nlohmann::json::parse(message);

        if (json.contains("alarm")) {
            sawAlarm = true;

            ASSERT_TRUE(json.at("alarm").is_object());
            EXPECT_TRUE(json.at("alarm").contains("type"));
            EXPECT_TRUE(json.at("alarm").contains("severity"));
        } else {
            sawNoAlarm = true;
        }

        if (sawAlarm && sawNoAlarm) {
            break;
        }
    }

    EXPECT_TRUE(sawAlarm);
    EXPECT_TRUE(sawNoAlarm);
}

}  // namespace