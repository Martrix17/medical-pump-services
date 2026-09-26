#ifndef SESSION_HPP
#define SESSION_HPP

#include <asio.hpp>
#include <asio/steady_timer.hpp>
#include <asio/streambuf.hpp>

#include "message_generator.hpp"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
  public:
    Session(tcp::socket socket, MessageGenerator& generator);

    void start();

  private:
    void sendMessage();
    void scheduleNext();

    tcp::socket socket_;
    MessageGenerator& generator_;
    asio::steady_timer timer_;
};

#endif  // SESSION_HPP