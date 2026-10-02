#include "simulator/tcp_session.hpp"

#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <system_error>

TcpSession::TcpSession(tcp::socket socket, IMessageGenerator& generator)
    : socket_(std::move(socket)), generator_(generator), timer_(socket_.get_executor()) {}

void TcpSession::start() {
    sendMessage();
}

void TcpSession::sendMessage() {
    auto self(shared_from_this());
    auto message = std::make_shared<std::string>(generator_.generate());

    asio::async_write(socket_, asio::buffer(*message),
                      [this, self, message](const std::error_code& ec, std::size_t /*length*/) {
                          if (ec) {
                              std::cerr << "TcpSession write failed: " << ec.message() << '\n';
                              return;
                          }
                          scheduleNext();
                      });
}

void TcpSession::scheduleNext() {
    auto self(shared_from_this());
    timer_.expires_after(std::chrono::seconds(2));
    timer_.async_wait([this, self](std::error_code ec) {
        if (!ec) {
            sendMessage();
        }
    });
}