#include "session.hpp"

#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <system_error>

Session::Session(tcp::socket socket, MessageGenerator& generator)
    : socket_(std::move(socket)), generator_(generator), timer_(socket_.get_executor()) {}

void Session::start() {
    sendMessage();
}

void Session::sendMessage() {
    auto self(shared_from_this());
    auto message = std::make_shared<std::string>(generator_.generate());

    asio::async_write(socket_, asio::buffer(*message),
                      [this, self, message](std::error_code ec, std::size_t /*length*/) {
                          if (ec) {
                              std::cerr << "Session write failed: " << ec.message() << '\n';
                              return;
                          }
                          scheduleNext();
                      });
}

void Session::scheduleNext() {
    auto self(shared_from_this());
    timer_.expires_after(std::chrono::seconds(2));
    timer_.async_wait([this, self](std::error_code ec) {
        if (!ec) {
            sendMessage();
        }
    });
}