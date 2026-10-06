#include <gtest/gtest.h>

#include "device/kafka/event_publisher.hpp"

TEST(KafkaEventPublisherTest, ConstructsWithValidConfiguration) {
    const char* broker = std::getenv("KAFKA_BROKER");
    const std::string kBroker = broker != nullptr ? broker : "kafka:29092";
    EXPECT_NO_THROW(EventPublisher publisher(kBroker, "device-events-test"));
}

TEST(KafkaEventPublisherTest, RejectsInvalidBrokerConfiguration) {
    EXPECT_THROW(EventPublisher publisher("", "device-events"), std::runtime_error);
}