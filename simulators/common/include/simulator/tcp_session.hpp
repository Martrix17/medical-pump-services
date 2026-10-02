#ifndef TCP_SESSION_HPP
#define TCP_SESSION_HPP

#include <asio.hpp>
#include <asio/steady_timer.hpp>
#include <asio/streambuf.hpp>

#include "interface_message_generator.hpp"

using asio::ip::tcp;

class TcpSession : public std::enable_shared_from_this<TcpSession> {
  public:
    TcpSession(tcp::socket socket, IMessageGenerator& generator);

    void start();

  private:
    void sendMessage();
    void scheduleNext();

    tcp::socket socket_;
    IMessageGenerator& generator_;
    asio::steady_timer timer_;
};

#endif  // TCP_SESSION_HPP