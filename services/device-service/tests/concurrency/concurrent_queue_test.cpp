#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "device/concurrency/concurrent_queue.hpp"

using namespace std::chrono_literals;

TEST(ConcurrentQueueTest, PushThenPopReturnsValue) {
    ConcurrentQueue<int> q;
    q.push(42);
    auto v = q.waitAndPop();
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, 42);
}

TEST(ConcurrentQueueTest, PreservesFifoOrder) {
    ConcurrentQueue<int> q;
    for (int i = 0; i < 5; ++i) {
        q.push(i);
    }

    for (int i = 0; i < 5; ++i) {
        auto v = q.waitAndPop();
        ASSERT_TRUE(v.has_value());
        EXPECT_EQ(*v, i);
    }
}

TEST(ConcurrentQueueTest, WaitAndPopBlocksUntilPush) {
    ConcurrentQueue<int> q;
    std::optional<int> result;
    std::thread consumer([&] { result = q.waitAndPop(); });

    std::this_thread::sleep_for(50ms);
    q.push(7);
    consumer.join();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 7);
}

TEST(ConcurrentQueueTest, StopUnblocksWaitingConsumerWithEmptyQueue) {
    ConcurrentQueue<int> q;
    std::optional<int> result;
    std::thread consumer([&] { result = q.waitAndPop(); });

    std::this_thread::sleep_for(50ms);
    q.stop();
    consumer.join();

    EXPECT_FALSE(result.has_value());
}

TEST(ConcurrentQueueTest, StopDoesNotDiscardAlreadyQueuedItems) {
    ConcurrentQueue<int> q;
    q.push(1);
    q.push(2);
    q.stop();

    // Existing items should still be drainable after stop().
    auto a = q.waitAndPop();
    auto b = q.waitAndPop();
    ASSERT_TRUE(a.has_value());
    ASSERT_TRUE(b.has_value());
    EXPECT_EQ(*a, 1);
    EXPECT_EQ(*b, 2);

    // Now empty + stopped -> nullopt.
    EXPECT_FALSE(q.waitAndPop().has_value());
}

TEST(ConcurrentQueueTest, MultipleConsumersEachGetDistinctItems) {
    ConcurrentQueue<int> q;
    constexpr int kItems = 1000;
    std::atomic<int> sum{0};
    std::atomic<int> popped{0};

    std::vector<std::thread> consumers;
    consumers.reserve(4);
    for (int i = 0; i < 4; ++i) {
        consumers.emplace_back([&] {
            while (auto v = q.waitAndPop()) {
                sum += *v;
                ++popped;
            }
        });
    }

    for (int i = 1; i <= kItems; ++i) {
        q.push(i);
    }

    std::this_thread::sleep_for(100ms);
    q.stop();

    for (auto& t : consumers) {
        t.join();
    }

    EXPECT_EQ(popped.load(), kItems);
    EXPECT_EQ(sum.load(), kItems * (kItems + 1) / 2);
}

TEST(ConcurrentQueueTest, SupportsMoveOnlyTypes) {
    ConcurrentQueue<std::unique_ptr<int>> q;
    q.push(std::make_unique<int>(99));
    auto v = q.waitAndPop();
    ASSERT_TRUE(v.has_value());
    ASSERT_NE(*v, nullptr);
    EXPECT_EQ(**v, 99);
}

TEST(ConcurrentQueueTest, StopIsIdempotentAndSafeToCallMultipleTimes) {
    ConcurrentQueue<int> q;
    q.stop();
    q.stop();
    EXPECT_FALSE(q.waitAndPop().has_value());
}