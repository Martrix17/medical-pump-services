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
                                             unsigned short port, IDeviceAdapter& adapter) {
    return std::shared_ptr<TcpClient>(new TcpClient(ioContext, std::move(host), port, adapter));
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
}

TcpClient::TcpClient(asio::io_context& ioContext, std::string host, unsigned short port,
                     IDeviceAdapter& adapter)
    : socket_(ioContext), resolver_(ioContext), reconnectTimer_(ioContext), host_(std::move(host)),
      port_(port), adapter_(adapter) {}

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
    reconnectTimer_.expires_after(reconnectDelay_);
    reconnectTimer_.async_wait([this, self](const asio::error_code& ec) {
        if (ec == asio::error::operation_aborted) {
            return;
        }
        reconnectDelay_ = std::min(reconnectDelay_ * 2, kMaxReconnectDelay_);
        connect();
    });
}

void TcpClient::read() {
    if (stopped_) {
        return;
    }

    auto self = shared_from_this();
    socket_.async_read_some(asio::buffer(readBuffer_),
                            [this, self](const asio::error_code& ec, std::size_t bytesTransferred) {
                                if (ec) {
                                    if (ec == asio::error::operation_aborted) {
                                        std::cerr << "Read failed for " << host_ << ":" << port_
                                                  << ": " << ec.message() << '\n';

                                        asio::error_code closeEc;
                                        socket_.close(closeEc);

                                        scheduleReconnect();
                                    }
                                    return;
                                }

                                receivedBuffer_.append(readBuffer_.data(), bytesTransferred);
                                processBuffer();
                                read();
                            });
}

void TcpClient::processBuffer() {
    constexpr std::string_view delimiter = "\n";

    std::size_t pos;
    while ((pos = receivedBuffer_.find(delimiter)) != std::string::npos) {
        std::string_view message(receivedBuffer_.data(), pos);
        adapter_.processMessage(message);
        // std::cout << "Processed message from " << adapter_.getDeviceID().value() << "\n";
        receivedBuffer_.erase(0, pos + delimiter.size());
    }
}