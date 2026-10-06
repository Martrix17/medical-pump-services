#include "device/kafka/event_publisher.hpp"

#include <librdkafka/rdkafkacpp.h>

#include <iostream>
#include <utility>

#include "device/kafka/event_serializer.hpp"

namespace {

std::string extractDeviceId(const DeviceEvent& event) {
    return std::visit([](const auto& concreteEvent) { return concreteEvent.deviceID().value(); },
                      event);
}

class DeliveryReport final : public RdKafka::DeliveryReportCb {
  public:
    void dr_cb(RdKafka::Message& message) override {
        if (message.err() != 0) {
            // Replace with the project's logger later.
            std::cerr << "Kafka delivery failed: " << message.errstr() << '\n';
        }
    }
};

DeliveryReport& deliveryReport() {
    static DeliveryReport callback;
    return callback;
}
}  // namespace

EventPublisher::EventPublisher(std::string brokers, std::string topic) : topic_(std::move(topic)) {
    if (brokers.empty()) {
        throw std::runtime_error("Kafka broker address must not be empty");
    }

    if (topic_.empty()) {
        throw std::runtime_error("Kafka topic must not be empty");
    }

    std::string error;

    auto* configuration = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    if (configuration == nullptr) {
        throw std::runtime_error("Failed to create Kafka configuration");
    }

    if (configuration->set("bootstrap.servers", brokers, error) != RdKafka::Conf::CONF_OK) {
        delete configuration;
        throw std::runtime_error("Failed to configure Kafka brokers: " + error);
    }

    if (configuration->set("dr_cb", &deliveryReport(), error) != RdKafka::Conf::CONF_OK) {
        delete configuration;
        throw std::runtime_error("Failed to configure Kafka delivery callback: " + error);
    }

    producer_ = std::unique_ptr<RdKafka::Producer>(RdKafka::Producer::create(configuration, error));

    delete configuration;

    if (!producer_) {
        throw std::runtime_error("Failed to create Kafka producer: " + error);
    }
}

EventPublisher::~EventPublisher() {
    if (producer_) {
        producer_->flush(5000);
    }
}

void EventPublisher::publish(DeviceEvent event) {
    const auto payload = EventSerializer::serialize(event);
    const auto key = extractDeviceId(event);

    const auto result = producer_->produce(
        topic_, RdKafka::Topic::PARTITION_UA, RdKafka::Producer::RK_MSG_COPY,
        const_cast<char*>(payload.data()), payload.size(), key.data(), key.size(), 0, nullptr);

    if (result != RdKafka::ERR_NO_ERROR) {
        std::cerr << "Kafka publish failed: " << RdKafka::err2str(result) << '\n';
    }

    // Allows librdkafka to process delivery callbacks.
    producer_->poll(0);
}