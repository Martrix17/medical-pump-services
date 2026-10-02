#include "device/concurrency/worker.hpp"

#include <utility>

Worker::Worker(ConcurrentQueue<std::string>& queue, IDeviceAdapter& adapter, IEventSink& eventSink)
    : queue_(queue), adapter_(adapter), eventSink_(eventSink) {}

void Worker::start() {
    thread_ = std::thread(&Worker::run, this);
}

void Worker::stop() {
    queue_.stop();
}

void Worker::join() {
    if (thread_.joinable()) {
        thread_.join();
    }
}

void Worker::run() {
    while (const auto message = queue_.waitAndPop()) {
        const auto events = adapter_.processMessage(*message);

        for (const auto& event : events) {
            eventSink_.publish(event);
        }
    }
}