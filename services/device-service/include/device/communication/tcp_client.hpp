#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include <array>
#include <asio.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <chrono>
#include <memory>

#include "device/adapter/interface_device_adapter.hpp"

using asio::ip::tcp;

class TcpClient : public std::enable_shared_from_this<TcpClient> {
  public:
    static std::shared_ptr<TcpClient> create(asio::io_context& ioContext, std::string host,
                                             unsigned short port, IDeviceAdapter& adapter);

    void start();
    void stop();

  private:
    TcpClient(asio::io_context& ioContext, std::string host, unsigned short port,
              IDeviceAdapter& adapter);

    void connect();
    void scheduleReconnect();
    void read();
    void processBuffer();

    tcp::socket socket_;
    tcp::resolver resolver_;
    asio::steady_timer reconnectTimer_;

    std::string host_;
    unsigned short port_;

    IDeviceAdapter& adapter_;
    std::string receivedBuffer_;
    std::array<char, 4096> readBuffer_;

    bool stopped_ = false;
    std::chrono::seconds reconnectDelay_{1};
    static constexpr std::chrono::seconds kMaxReconnectDelay_{30};
};

#endif  // TCP_CLIENT_HPP