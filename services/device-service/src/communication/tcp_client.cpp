#include "device/communication/tcp_client.hpp"

#include <asio/buffer.hpp>
#include <asio/connect.hpp>
#include <asio/error.hpp>
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>

std::shared_ptr<TcpClient> TcpClient::create(asio::io_context& ioContext, std::string host,
                                             unsigned short port, IFramer& framer,
                                             ConcurrentQueue<std::string>& messageQueue) {
    return std::shared_ptr<TcpClient>(
        new TcpClient(ioContext, std::move(host), port, framer, messageQueue));
}

void TcpClient::start() {
    connect();
}

void TcpClient::stop() {
    stopped_ = true;

    asio::error_code ec;
    reconnectTimer_.cancel();
    socket_.cancel(ec);
    socket_.close(ec);

    framer_.reset();
}

TcpClient::TcpClient(asio::io_context& ioContext, std::string host, unsigned short port,
                     IFramer& framer, ConcurrentQueue<std::string>& messageQueue)
    : socket_(ioContext), resolver_(ioContext), reconnectTimer_(ioContext), host_(std::move(host)),
      port_(port), framer_(framer), messageQueue_(messageQueue) {}

void TcpClient::connect() {
    if (stopped_) {
        return;
    }

    auto self = shared_from_this();
    resolver_.async_resolve(
        host_, std::to_string(port_),
        [this, self](const asio::error_code& ec, const tcp::resolver::results_type& endpoints) {
            if (ec) {
                std::cerr << "Resolve failed for " << host_ << ":" << port_ << ": " << ec.message()
                          << "\n";
                scheduleReconnect();
                return;
            }

            asio::async_connect(
                socket_, endpoints,
                [this, self](const asio::error_code& ec, const tcp::endpoint& /*endpoint*/) {
                    if (ec) {
                        std::cerr << "Connect failed for " << host_ << ":" << port_ << ": "
                                  << ec.message() << "\n";
                        scheduleReconnect();
                        return;
                    }
                    reconnectDelay_ = std::chrono::seconds{1};

                    read();
                });
        });
}

void TcpClient::scheduleReconnect() {
    if (stopped_) {
        return;
    }

    auto self = shared_from_this();

    const auto delay = reconnectDelay_;

    reconnectTimer_.expires_after(delay);
    reconnectTimer_.async_wait([this, self](const asio::error_code& ec) {
        if (ec == asio::error::operation_aborted) {
            return;
        }

        connect();
        reconnectDelay_ = std::min(reconnectDelay_ * 2, kMaxReconnectDelay_);
    });
}

void TcpClient::read() {
    if (stopped_) {
        return;
    }

    auto self = shared_from_this();
    socket_.async_read_some(asio::buffer(readBuffer_), [this, self](const asio::error_code& ec,
                                                                    std::size_t bytesTransferred) {
        if (ec) {
            if (ec != asio::error::operation_aborted) {
                std::cerr << "Read failed for " << host_ << ":" << port_ << ": " << ec.message()
                          << '\n';

                framer_.reset();

                asio::error_code closeEc;
                socket_.close(closeEc);

                scheduleReconnect();
            }
            return;
        }

        for (auto message :
             framer_.process(std::string_view(readBuffer_.data(), bytesTransferred))) {
            messageQueue_.push(std::move(message));
        }

        read();
    });
}
