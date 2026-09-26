#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <exception>
#include <iostream>

#include "message_generator.hpp"
#include "tcp_server.hpp"

int main(int argc, char** argv) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <port>\n";
            return 1;
        }

        asio::io_context ioContext;
        MessageGenerator generator{"pump-a-001"};

        TcpServer server(ioContext, static_cast<unsigned short>(std::atoi(argv[1])), generator);
        server.start();

        std::cout << "Server listening on port: " << argv[1] << "\n";
        ioContext.run();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}