#include <gtest/gtest.h>

#include <asio.hpp>
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <variant>

#include "device/adapter/pump_b_adapter.hpp"
#include "device/common/binary_reader.hpp"
#include "device/communication/fixed_size_framer.hpp"
#include "device/communication/tcp_client.hpp"
#include "device/concurrency/concurrent_queue.hpp"
#include "device/concurrency/worker.hpp"
#include "test_event_sink.hpp"

std::string makeValidPumpBFrame() {
    std::string frame;
    frame.reserve(28);

    // Header
    frame.push_back(static_cast<char>(0xAA));
    frame.push_back(static_cast<char>(0x55));

    // Message type: measurement
    frame.push_back(static_cast<char>(0x02));

    // Status: connected
    frame.push_back(static_cast<char>(0x02));

    // Device ID: 0x00000001
    binary::appendUint32(frame, 1234);

    // Timestamp
    binary::appendUint64(frame, 1758499200);

    // Flow rate = 1.5f
    binary::appendFloat32(frame, 1.5f);

    // Pressure = 2.1f
    binary::appendFloat32(frame, 2.1f);

    // Alarm type = none
    frame.push_back(0x00);

    // Severity = none
    frame.push_back(0x00);

    const auto checksum = binary::calculateChecksum(frame);

    binary::appendUint16(frame, checksum);

    EXPECT_EQ(frame.size(), 28);

    return frame;
}

TEST(PumpBIntegrationTest, ValidMessageProducesMeasurementEvent) {
    asio::io_context ioContext;

    FixedSizeFramer framer{28};
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

    const auto frame = makeValidPumpBFrame();

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

    FixedSizeFramer framer{28};
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

    const auto validFrame = makeValidPumpBFrame();

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