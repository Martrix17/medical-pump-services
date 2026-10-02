#include <gtest/gtest.h>

#include <map>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "device/adapter/interface_device_adapter.hpp"
#include "device/concurrency/concurrent_queue.hpp"
#include "device/concurrency/worker.hpp"
#include "device/domain/device_id.hpp"
#include "device/domain/device_status.hpp"
#include "device/event/interface_event_sink.hpp"

namespace {

DeviceEvent makeMeasurement(double flowRate) {
    return Measurement{DeviceID{"pump-1"}, flowRate, 1.0, Timestamp::clock::now()};
}

double flowRateOf(const DeviceEvent& event) {
    return std::get<Measurement>(event).flowRate();
}

// Scripted adapter: returns preconfigured events per raw message, records inputs.
// Only touched from the worker thread while running; read after join() (happens-before).
class FakeAdapter : public IDeviceAdapter {
  public:
    std::vector<DeviceEvent> processMessage(std::string_view rawMessage) override {
        receivedMessages.emplace_back(rawMessage);
        if (auto it = responses.find(std::string(rawMessage)); it != responses.end()) {
            return it->second;
        }
        return {};
    }

    [[nodiscard]] DeviceID getDeviceID() const override {
        return DeviceID{"stub"};
    }
    [[nodiscard]] std::optional<DeviceStatus> getStatus() const override {
        return std::nullopt;
    }
    [[nodiscard]] std::vector<Alarm> getAlarms() const override {
        return {};
    }
    [[nodiscard]] std::optional<Measurement> getMeasurement() const override {
        return std::nullopt;
    }

    std::map<std::string, std::vector<DeviceEvent>> responses;
    std::vector<std::string> receivedMessages;
};

class RecordingSink : public IEventSink {
  public:
    void publish(DeviceEvent event) override {
        events.push_back(event);
    }

    std::vector<DeviceEvent> events;
};

class WorkerTest : public ::testing::Test {
  protected:
    ConcurrentQueue<std::string> queue;
    FakeAdapter adapter;
    RecordingSink sink;
};

}  // namespace

TEST_F(WorkerTest, ForwardsAdapterEventsToSinkInOrder) {
    adapter.responses["msg"] = {makeMeasurement(1.0), makeMeasurement(2.0)};

    Worker worker(queue, adapter, sink);
    worker.start();
    queue.push("msg");
    worker.stop();
    worker.join();

    ASSERT_EQ(sink.events.size(), 2U);
    EXPECT_DOUBLE_EQ(flowRateOf(sink.events[0]), 1.0);
    EXPECT_DOUBLE_EQ(flowRateOf(sink.events[1]), 2.0);
}

TEST_F(WorkerTest, PassesRawMessagesToAdapterUnchangedAndInOrder) {
    Worker worker(queue, adapter, sink);
    worker.start();
    queue.push("first");
    queue.push("second");
    queue.push("third");
    worker.stop();
    worker.join();

    EXPECT_EQ(adapter.receivedMessages, (std::vector<std::string>{"first", "second", "third"}));
}

TEST_F(WorkerTest, PublishesNothingWhenAdapterReturnsNoEvents) {
    Worker worker(queue, adapter, sink);
    worker.start();
    queue.push("garbage");
    worker.stop();
    worker.join();

    EXPECT_EQ(adapter.receivedMessages.size(), 1U);
    EXPECT_TRUE(sink.events.empty());
}

TEST_F(WorkerTest, KeepsProcessingAfterMessageYieldingNoEvents) {
    adapter.responses["good"] = {makeMeasurement(5.0)};

    Worker worker(queue, adapter, sink);
    worker.start();
    queue.push("garbage");
    queue.push("good");
    worker.stop();
    worker.join();

    ASSERT_EQ(sink.events.size(), 1U);
    EXPECT_DOUBLE_EQ(flowRateOf(sink.events[0]), 5.0);
}

TEST_F(WorkerTest, StopUnblocksIdleWorker) {
    Worker worker(queue, adapter, sink);
    worker.start();
    worker.stop();
    worker.join();

    EXPECT_TRUE(adapter.receivedMessages.empty());
}

TEST_F(WorkerTest, DrainsAlreadyQueuedMessagesBeforeExitingOnStop) {
    for (int i = 0; i < 100; ++i) {
        adapter.responses[std::to_string(i)] = {makeMeasurement(i)};
        queue.push(std::to_string(i));
    }
    queue.stop();

    Worker worker(queue, adapter, sink);
    worker.start();
    worker.join();

    ASSERT_EQ(sink.events.size(), 100U);
    for (int i = 0; i < 100; ++i) {
        EXPECT_DOUBLE_EQ(flowRateOf(sink.events[i]), i);
    }
}

TEST_F(WorkerTest, JoinWithoutStartIsSafe) {
    Worker worker(queue, adapter, sink);
    worker.join();
}

TEST_F(WorkerTest, JoinIsIdempotent) {
    Worker worker(queue, adapter, sink);
    worker.start();
    worker.stop();
    worker.join();
    worker.join();
}

TEST(WorkerMultiTest, TwoWorkersSharingQueueProcessEachMessageExactlyOnce) {
    constexpr int kMessages = 1000;

    ConcurrentQueue<std::string> queue;
    FakeAdapter adapterA;
    FakeAdapter adapterB;
    RecordingSink sinkA;
    RecordingSink sinkB;

    for (int i = 0; i < kMessages; ++i) {
        const auto key = std::to_string(i);
        adapterA.responses[key] = {makeMeasurement(i)};
        adapterB.responses[key] = {makeMeasurement(i)};
    }

    Worker a(queue, adapterA, sinkA);
    Worker b(queue, adapterB, sinkB);
    a.start();
    b.start();

    for (int i = 0; i < kMessages; ++i) {
        queue.push(std::to_string(i));
    }

    queue.stop();
    a.join();
    b.join();

    EXPECT_EQ(sinkA.events.size() + sinkB.events.size(), static_cast<size_t>(kMessages));

    std::vector<bool> seen(kMessages, false);
    for (const auto* sink : {&sinkA, &sinkB}) {
        for (const auto& event : sink->events) {
            const auto idx = static_cast<size_t>(flowRateOf(event));
            EXPECT_FALSE(seen[idx]) << "message " << idx << " processed twice";
            seen[idx] = true;
        }
    }
}
