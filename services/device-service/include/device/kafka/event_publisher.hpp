#ifndef EVENT_PUBLISHER_HPP
#define EVENT_PUBLISHER_HPP

#include <memory>
#include <string>

#include "device/event/interface_event_sink.hpp"

namespace RdKafka {
class Producer;
class Topic;
}  // namespace RdKafka

class EventPublisher final : public IEventSink {
  public:
    EventPublisher(std::string brokers, std::string topic);

    ~EventPublisher() override;

    EventPublisher(const EventPublisher&) = delete;
    EventPublisher& operator=(const EventPublisher&) = delete;

    void publish(DeviceEvent event) override;

  private:
    std::string topic_;
    std::unique_ptr<RdKafka::Producer> producer_;
};

#endif  // EVENT_PUBLISHER_HPP