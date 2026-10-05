#include <charconv>
#include <chrono>
#include <iostream>
#include <memory>

#include "device/adapter/pump_a_adapter.hpp"
#include "device/adapter/pump_b_adapter.hpp"
#include "device/communication/fixed_size_framer.hpp"
#include "device/communication/interface_framer.hpp"
#include "device/communication/line_framer.hpp"
#include "device/communication/tcp_client.hpp"
#include "device/concurrency/concurrent_queue.hpp"
#include "device/concurrency/worker.hpp"
#include "device/event/console_event_sink.hpp"

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
        return 1;
    }

    const std::string host = argv[1];

    unsigned short port{};
    const std::string_view portArg = argv[2];

    auto [ptr, ec] = std::from_chars(portArg.data(), portArg.data() + portArg.size(), port);
    if (ec != std::errc{} || ptr != portArg.data() + portArg.size()) {
        std::cerr << "Invalid port: " << argv[2] << '\n';
        return 1;
    }

    std::unique_ptr<IDeviceAdapter> adapter;
    std::unique_ptr<IFramer> framer;
    if (argv[3] == std::string("PumpA")) {
        adapter = std::make_unique<PumpAAdapter>();
        framer = std::make_unique<LineFramer>();
    } else if (argv[3] == std::string("PumpB")) {
        adapter = std::make_unique<PumpBAdapter>();
        framer = std::make_unique<FixedSizeFramer>();
    } else {
        std::cerr << "Invalid adapter/framer type: " << std::string(argv[3]) << '\n';
        return 2;
    }

    try {
        asio::io_context ioContext;
        asio::signal_set signals(ioContext, SIGINT, SIGTERM);

        ConcurrentQueue<std::string> messageQueue;
        ConsoleEventSink eventSink;

        Worker worker(messageQueue, *adapter, eventSink);

        auto client = TcpClient::create(ioContext, host, port, *framer, messageQueue);

        signals.async_wait([&](const asio::error_code& /*ec*/, int /*signal*/) {
            std::cout << "Shutting down...\n";
            client->stop();
            ioContext.stop();
        });

        worker.start();
        client->start();

        std::cout << "Client connecting to port: " << host << ":" << port << "\n";

        ioContext.run();

        worker.stop();
        worker.join();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}