#ifndef SEKA_SERVER_SERVER_H
#define SEKA_SERVER_SERVER_H

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>

#include "core/secure_rng.h"
#include "server/server_state.h"

namespace seka::server {

class Server {
  public:
    Server(boost::asio::io_context& ioc, const boost::asio::ip::tcp::endpoint& endpoint);

    void run();

  private:
    boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::unique_ptr<core::SecureRng> rng_;
    ServerState state_;

    void do_accept();
    void on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
};

}  // namespace seka::server

#endif  // SEKA_SERVER_SERVER_H
