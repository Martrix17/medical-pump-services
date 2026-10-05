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

namespace {

struct TestPipeline {
    asio::io_context& ioContext;

    FixedSizeFramer framer{helper::pumpB::PumpBFrameSize};
    ConcurrentQueue<std::string> queue;
    PumpBAdapter adapter;
    TestEventSink eventSink;

    Worker worker;
    std::shared_ptr<TcpClient> client;

    asio::ip::tcp::acceptor acceptor;
    asio::ip::tcp::socket serverSocket;

    explicit TestPipeline(asio::io_context& context)
        : ioContext(context), worker(queue, adapter, eventSink),
          acceptor(context, {asio::ip::tcp::v4(), 0}), serverSocket(context) {}
    [[nodiscard]] unsigned short port() const {
        return acceptor.local_endpoint().port();
    }

    void start() {
        worker.start();
        client = TcpClient::create(ioContext, "127.0.0.1", port(), framer, queue);
        client->start();
    }

    void acceptConnection() {
        acceptor.accept(serverSocket);
    }

    void send(const std::string& frame) {
        asio::write(serverSocket, asio::buffer(frame));
    }

    void shutdown() {
        client->stop();
        queue.stop();
        worker.join();
        serverSocket.close();
        acceptor.close();
    }
};

}  // namespace

TEST(WorkerLifecycleIntegrationTest, MultipleWorkersTerminateCleanlyDuringShutdown) {
    asio::io_context ioContext;

    TestPipeline pipelineA{ioContext};
    TestPipeline pipelineB{ioContext};

    pipelineA.start();
    pipelineB.start();

    std::thread ioThread{[&ioContext] { ioContext.run(); }};

    // Accept both client connections.
    pipelineA.acceptConnection();
    pipelineB.acceptConnection();

    // Send one message through each independent pipeline.
    const auto frameA = helper::pumpB::makeValidPumpBFrame();
    const auto frameB = helper::pumpB::makeValidPumpBFrame();

    pipelineA.send(frameA);
    pipelineB.send(frameB);

    // Both workers must process their respective messages.
    ASSERT_TRUE(pipelineA.eventSink.waitForEvent(std::chrono::seconds{2}));
    ASSERT_TRUE(pipelineB.eventSink.waitForEvent(std::chrono::seconds{2}));

    ASSERT_TRUE(pipelineA.eventSink.event().has_value());
    ASSERT_TRUE(pipelineB.eventSink.event().has_value());

    // -------------------------------------------------------------------------
    // Shutdown both independent pipelines.
    // -------------------------------------------------------------------------

    pipelineA.client->stop();
    pipelineB.client->stop();

    pipelineA.queue.stop();
    pipelineB.queue.stop();

    // Both workers must wake up and terminate.
    pipelineA.worker.join();
    pipelineB.worker.join();

    // Stop the Asio event loop after the clients have been stopped.
    ioContext.stop();

    if (ioThread.joinable()) {
        ioThread.join();
    }

    pipelineA.serverSocket.close();
    pipelineB.serverSocket.close();

    pipelineA.acceptor.close();
    pipelineB.acceptor.close();
}