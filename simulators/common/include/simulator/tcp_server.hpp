#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <asio.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#include "interface_message_generator.hpp"

using asio::ip::tcp;

class TcpServer {
  public:
    TcpServer(asio::io_context& ioContext, unsigned short port, IMessageGenerator& generator);

    void start();

  private:
    void accept();

    tcp::acceptor acceptor_;
    IMessageGenerator& generator_;
};

#endif  // TCP_SERVER_HPP