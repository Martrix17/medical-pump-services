#include <gtest/gtest.h>
#include <librdkafka/rdkafkacpp.h>

#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "device/domain/device_id.hpp"
#include "device/domain/event.hpp"
#include "device/domain/measurement.hpp"
#include "device/kafka/event_publisher.hpp"

namespace {

const char* broker = std::getenv("KAFKA_BROKER");
const std::string kBroker = broker != nullptr ? broker : "kafka:29092";
constexpr auto kTopic = "device-events-test";
const auto groupID = "device-service-test-" +
                     std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());

class KafkaPublisherIntegrationTest : public ::testing::Test {
  protected:
    void SetUp() override {
        std::string error;

        auto* config = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

        ASSERT_NE(config, nullptr);

        ASSERT_EQ(config->set("bootstrap.servers", kBroker, error), RdKafka::Conf::CONF_OK)
            << error;

        ASSERT_EQ(config->set("group.id", groupID, error), RdKafka::Conf::CONF_OK) << error;

        ASSERT_EQ(config->set("auto.offset.reset", "earliest", error), RdKafka::Conf::CONF_OK)
            << error;

        consumer_.reset(RdKafka::KafkaConsumer::create(config, error));

        delete config;

        ASSERT_NE(consumer_, nullptr) << error;

        ASSERT_EQ(consumer_->subscribe({kTopic}), RdKafka::ERR_NO_ERROR);
    }

    void TearDown() override {
        if (consumer_) {
            consumer_->close();
        }
    }

    std::unique_ptr<RdKafka::KafkaConsumer> consumer_;
};

TEST_F(KafkaPublisherIntegrationTest, PublishesEventWithDeviceIdAsKey) {
    EventPublisher publisher(kBroker, kTopic);

    const DeviceID deviceId("pump-test-001");

    const Measurement measurement(deviceId, 1.5, 2.1, std::chrono::system_clock::now());

    {
        EventPublisher publisher(kBroker, kTopic);
        publisher.publish(DeviceEvent{measurement});
    }  // KafkaEventPublisher destructor flushes pending messages here.

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

    RdKafka::Message* message = nullptr;

    while (std::chrono::steady_clock::now() < deadline) {
        message = consumer_->consume(100);

        if (message->err() == RdKafka::ERR_NO_ERROR) {
            break;
        }

        delete message;
        message = nullptr;
    }

    ASSERT_NE(message, nullptr);
    ASSERT_EQ(message->err(), RdKafka::ERR_NO_ERROR);

    ASSERT_NE(message->key(), nullptr);
    EXPECT_EQ(*message->key(), "pump-test-001");

    ASSERT_NE(message->payload(), nullptr);

    const std::string payload(static_cast<const char*>(message->payload()), message->len());

    delete message;

    const auto json = nlohmann::json::parse(payload);

    EXPECT_EQ(json.at("event_type"), "measurement");
    EXPECT_EQ(json.at("device_id"), "pump-test-001");
    EXPECT_DOUBLE_EQ(json.at("flow_rate"), 1.5);
    EXPECT_DOUBLE_EQ(json.at("pressure"), 2.1);
}

}  // namespace
