#ifndef TEST_EVENT_SINK_HPP
#define TEST_EVENT_SINK_HPP

#include <condition_variable>
#include <mutex>
#include <optional>

#include "device/event/interface_event_sink.hpp"

class TestEventSink final : public IEventSink {
  public:
    void publish(DeviceEvent event) override {
        {
            std::lock_guard lock(mutex_);
            event_ = std::move(event);
        }

        condition_.notify_one();
    }

    template <typename Rep, typename Period>
    bool waitForEvent(std::chrono::duration<Rep, Period> timeout) {
        std::unique_lock lock(mutex_);

        return condition_.wait_for(lock, timeout, [this] { return event_.has_value(); });
    }

    std::optional<DeviceEvent> event() {
        std::lock_guard lock(mutex_);
        return event_;
    }

  private:
    std::mutex mutex_;
    std::condition_variable condition_;
    std::optional<DeviceEvent> event_;
};

#endif  // TEST_EVENT_SINK_HPP