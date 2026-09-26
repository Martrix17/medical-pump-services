#include "tcp_server.hpp"

#include <iostream>

#include "session.hpp"

TcpServer::TcpServer(asio::io_context& ioContext, unsigned short port, MessageGenerator& generator)
    : acceptor_(ioContext, tcp::endpoint(tcp::v4(), port)), generator_(generator) {}

void TcpServer::start() {
    accept();
}

void TcpServer::accept() {
    acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::cout << "Client connected: " << socket.remote_endpoint() << "\n";
            std::make_shared<Session>(std::move(socket), generator_)->start();
        }
        accept();
    });
}