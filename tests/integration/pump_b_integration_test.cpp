#include <gtest/gtest.h>

#include <asio.hpp>
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <variant>

#include "device/adapter/pump_b_adapter.hpp"
#include "device/communication/fixed_size_framer.hpp"
#include "device/communication/tcp_client.hpp"
#include "device/concurrency/concurrent_queue.hpp"
#include "device/concurrency/worker.hpp"
#include "pump_helper.hpp"
#include "test_event_sink.hpp"

TEST(PumpBIntegrationTest, ValidMessageProducesMeasurementEvent) {
    asio::io_context ioContext;

    FixedSizeFramer framer{helper::pumpB::PumpBFrameSize};
    ConcurrentQueue<std::string> queue;

    PumpBAdapter adapter;
    TestEventSink eventSink;

    auto worker = std::make_unique<Worker>(queue, adapter, eventSink);
    worker->start();

    // Start local test TCP server.
    asio::ip::tcp::acceptor acceptor{ioContext, {asio::ip::tcp::v4(), 0}};

    const auto port = acceptor.local_endpoint().port();

    auto client = TcpClient::create(ioContext, "127.0.0.1", port, framer, queue);

    client->start();

    std::thread ioThread([&ioContext] { ioContext.run(); });

    // Accept the connection and send one known Pump B frame.
    asio::ip::tcp::socket socket{ioContext};

    acceptor.accept(socket);

    const auto frame = helper::pumpB::makeValidPumpBFrame();

    asio::write(socket, asio::buffer(frame));

    ASSERT_TRUE(eventSink.waitForEvent(std::chrono::seconds(2)));

    const auto event = eventSink.event();

    ASSERT_TRUE(event.has_value());
    ASSERT_TRUE(std::holds_alternative<Measurement>(*event));

    const auto& measurement = std::get<Measurement>(*event);

    EXPECT_EQ(measurement.deviceID(), DeviceID{"1234"});

    EXPECT_DOUBLE_EQ(measurement.flowRate(), 1.5F);
    EXPECT_DOUBLE_EQ(measurement.pressure(), 2.1F);

    client->stop();
    queue.stop();

    ioContext.stop();

    worker->join();
    ioThread.join();
}

TEST(PumpBIntegrationTest, InvalidFrameDoesNotProduceEvent) {
    asio::io_context ioContext;

    FixedSizeFramer framer{helper::pumpB::PumpBFrameSize};
    ConcurrentQueue<std::string> queue;

    PumpBAdapter adapter;
    TestEventSink eventSink;

    auto worker = std::make_unique<Worker>(queue, adapter, eventSink);

    worker->start();

    // Start local test TCP server.
    asio::ip::tcp::acceptor acceptor{ioContext, {asio::ip::tcp::v4(), 0}};

    const auto port = acceptor.local_endpoint().port();

    auto client = TcpClient::create(ioContext, "127.0.0.1", port, framer, queue);

    client->start();

    std::thread ioThread([&ioContext] { ioContext.run(); });

    // Accept the connection and send one known Pump B frame.
    asio::ip::tcp::socket socket{ioContext};

    acceptor.accept(socket);

    const auto validFrame = helper::pumpB::makeValidPumpBFrame();

    auto invalidFrame = validFrame;

    // Corrupt checksum.
    invalidFrame[27] ^= 0xFF;

    // Send invalid frame.
    asio::write(socket, asio::buffer(invalidFrame));

    // Give the worker enough time to process it.
    EXPECT_FALSE(eventSink.waitForEvent(std::chrono::milliseconds(500)));

    // Verify no event was published.
    const auto event = eventSink.event();

    ASSERT_FALSE(event.has_value());

    client->stop();
    queue.stop();

    ioContext.stop();

    worker->join();
    ioThread.join();
}