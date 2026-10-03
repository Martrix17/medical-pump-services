#include <gtest/gtest.h>

#include <asio.hpp>
#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <variant>

#include "device/adapter/pump_a_adapter.hpp"
#include "device/communication/line_framer.hpp"
#include "device/communication/tcp_client.hpp"
#include "device/concurrency/concurrent_queue.hpp"
#include "device/concurrency/worker.hpp"
#include "test_event_sink.hpp"

namespace {

using json = nlohmann::json;

constexpr std::string_view validPumpAFrame = R"({
    "device_id": "1234",
    "timestamp": "2026-09-22T00:00:00Z",
    "status": "Connected",
    "measurement": {   
        "flow_rate": 1.5,
        "pressure": 2.1
    }
})";

constexpr std::string_view invalidPumpAFrame = R"({
    "device_id": "1234",
    "timestamp": "2026-09-22T00:00:00Z",
    "status": "Connected",
    "measurement": {   
        "flow_rate": "not-a-number",
        "pressure": 2.1
    }
})";

std::string makePumpAFrame(std::string_view rawMessage) {
    json message = json::parse(rawMessage);
    return message.dump() + "\n";
}

}  // namespace

TEST(PumpAIntegrationTest, ValidMessageProducesMeasurementEvent) {
    asio::io_context ioContext;

    LineFramer framer;
    ConcurrentQueue<std::string> queue;

    PumpAAdapter adapter;
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

    const auto frame = makePumpAFrame(validPumpAFrame);

    asio::write(socket, asio::buffer(frame));

    ASSERT_TRUE(eventSink.waitForEvent(std::chrono::seconds(2)));

    const auto event = eventSink.event();

    ASSERT_TRUE(event.has_value());
    ASSERT_TRUE(std::holds_alternative<Measurement>(*event));

    const auto& measurement = std::get<Measurement>(*event);

    EXPECT_EQ(measurement.deviceID(), DeviceID{"1234"});

    EXPECT_DOUBLE_EQ(measurement.flowRate(), 1.5);
    EXPECT_DOUBLE_EQ(measurement.pressure(), 2.1);

    client->stop();
    queue.stop();

    ioContext.stop();

    worker->join();
    ioThread.join();
}

TEST(PumpAIntegrationTest, InvalidFrameDoesNotProduceEvent) {
    asio::io_context ioContext;

    LineFramer framer;
    ConcurrentQueue<std::string> queue;

    PumpAAdapter adapter;
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

    const auto frame = makePumpAFrame(invalidPumpAFrame);

    // Send invalid frame.
    asio::write(socket, asio::buffer(frame));

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