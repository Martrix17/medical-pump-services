#include <charconv>
#include <chrono>
#include <iostream>

#include "device/adapter/pump_a_adapter.hpp"
#include "device/communication/tcp_client.hpp"

int main(int argc, char** argv) {
    if (argc != 3) {
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

    try {
        asio::io_context ioContext;

        asio::signal_set signals(ioContext, SIGINT, SIGTERM);

        PumpAAdapter adapter;
        auto client = TcpClient::create(ioContext, host, port, adapter);

        signals.async_wait([&](const asio::error_code& /*ec*/, int /*signal*/) {
            std::cout << "Shutting down...\n";
            client->stop();
            ioContext.stop();
        });

        client->start();
        std::cout << "Client listening on port: " << port << "\n";
        ioContext.run();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}