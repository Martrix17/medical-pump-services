#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <charconv>
#include <exception>
#include <iostream>

#include "pump_b_message_generator.hpp"
#include "simulator/tcp_server.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }

    unsigned short port{};
    const std::string_view portArg = argv[1];
    auto [ptr, ec] = std::from_chars(portArg.data(), portArg.data() + portArg.size(), port);
    if (ec != std::errc{} || ptr != portArg.data() + portArg.size()) {
        std::cerr << "Invalid port: " << argv[1] << '\n';
        return 1;
    }

    try {
        asio::io_context ioContext;
        PumpBMessageGenerator generator{12345};

        TcpServer server(ioContext, port, generator);
        server.start();

        std::cout << "Server listening on port: " << port << "\n";
        ioContext.run();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}